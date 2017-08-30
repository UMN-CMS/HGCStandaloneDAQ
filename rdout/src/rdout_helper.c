// gcc -O rdout_helper.c ejf_rdout.c ctl_orm.c data_orm.c hexbd.c hexbd_config.c spi_common.c -l bcm2835 -o rdout_helper

#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "hexbd_config.h"
#include "ctl_orm.h"
#include "data_orm.h"
#include "ejf_rdout.h"
#include "spi_common.h"


#define doPwrCycle 0



//========================================================================
// MAIN
//========================================================================

int main(int argc, char *argv[]) {
    int res;
    int i, k, hexbd;
    int isFifoEmpty;

    int hx;
    int junk[2000];

    // Setting up number of run events, file name, etc
    int PED = 0;

    if( argc != 2 ){
        fprintf(stderr,"Incorrect arguments: <PED>\n");
        return 0 ;
    }

    PED = atoi(argv[1]);

    // Startup the SPI interface on the Pi.
    init_spi();

    // Power cycle the ORMs.
    if (doPwrCycle) {

        for(i = 0; i < 5; i++) {
            if(i == 4)
                fprintf(stderr, "power cycle orm: data_%i...", i);
            else
                fprintf(stderr,"power cycle orm: ctl...");
            power_cycle(i);
            fprintf(stderr, "done.\n");
            sleep(1);
        }

        fprintf(stderr,"sleeping for 10s...");
        sleep(10);
        fprintf(stderr,"done.\n");
    }

    // Set the date stamp to zero.
    int date_stamp0, date_stamp1;
    CTL_put_date_stamp0(0);// To be used as Trigger_Send_OK
    CTL_put_date_stamp1(0);
    date_stamp0 =  CTL_get_date_stamp0();
    date_stamp1 =  CTL_get_date_stamp1();
    fprintf(stderr,"date_stamp = 0x%04x 0x%04x\n",
            (int)date_stamp1, (int)date_stamp0);

    // Reset everything. twice...
    for(i = 0; i < 2; i++) {
        for(k = 0; k < 8; k++) {
            DATA_reset_all(k);
        }
        CTL_reset_all();
    }

    // Get the firmware version.
    int version;
    version = CTL_get_firmware_version();
    fprintf(stderr,"ctl firmware version = 0x%04x\n",(int)version);
    version = DATA_get_firmware_version(0);
    fprintf(stderr,"orm_0 firmware version = 0x%04x\n",(int)version);
    version = DATA_get_firmware_version(1);
    fprintf(stderr,"orm_1 firmware version = 0x%04x\n",(int)version);
    version = DATA_get_firmware_version(2);
    fprintf(stderr,"orm_2 firmware version = 0x%04x\n",(int)version);
    version = DATA_get_firmware_version(3);
    fprintf(stderr,"orm_3 firmware version = 0x%04x\n",(int)version);

    // Test the constant registers, for debug.
    int constant0, constant1;
    constant0 = CTL_get_constant0();
    constant1 = CTL_get_constant1();
    fprintf(stderr,"CTL constant = 0x%04x 0x%04x\n",
            (int)constant1, (int)constant0);

    // Test the dummy registers, for debug.
    int dummy0, dummy1;
    CTL_put_dummy0(0xABCD);
    CTL_put_dummy1(0x1234);
    dummy0 = CTL_get_dummy0();
    dummy1 = CTL_get_dummy1();
    fprintf(stderr,"dummy = 0x%04x 0x%04x\n",
            (int)dummy1, (int)dummy0);

    // Get the default skiroc fifo block_size.
    int block_size;
    block_size = CTL_get_block_size();
    fprintf(stderr,"block_size = %d\n", (int)block_size);

    // empty local fifo by forcing extra reads, ignore results
    fprintf(stderr,"emptying local fifos (partially)...");
    for (hx=0; hx<MAXHEXBDS; hx++) HEXBD_read1000_local_fifo(hx,junk);
    for (hx=0; hx<MAXHEXBDS; hx++) HEXBD_read1000_local_fifo(hx,junk);
    for (hx=0; hx<MAXHEXBDS; hx++) HEXBD_read1000_local_fifo(hx,junk);
    fprintf(stderr,"done.\n");

    // Run a test on each of the8 hexaboards looking for good communication.
    int hexbd_mask;
    hexbd_mask = HEXBD_verify_communication(1);
    fprintf(stderr,"hexbd_mask = 0x%02x\n",(int)hexbd_mask);
    if(hexbd_mask == 0) {
        fprintf(stderr, "hexbd_mask is 0. Aborting\n");
        return 1;
    }

    // Set the skiroc mask.
    int skiroc_mask0, skiroc_mask1;
    skiroc_mask0 = 0;
    for (hx=0; hx<4; hx++) {
        if ((hexbd_mask & (1<<hx)) != 0) skiroc_mask0 |= 0xF << (4*hx);
    }
    skiroc_mask1 = 0;
    for (hx=4; hx<8; hx++) {
        if ((hexbd_mask & (1<<hx)) != 0) skiroc_mask1 |= 0xF << ((4*hx)-16);
    }
    CTL_put_skiroc_mask1(skiroc_mask1);
    CTL_put_skiroc_mask0(skiroc_mask0);

    // Get the skiroc mask.
    skiroc_mask1 = CTL_get_skiroc_mask1();
    skiroc_mask0 = CTL_get_skiroc_mask0();
    fprintf(stderr,"skiroc_mask = 0x%04x 0x%04x\n",
            (int)skiroc_mask1, (int)skiroc_mask0);

    // Configure the active hexaboards here, before enabling the 
    // automatic xfer mechanism (which ignores hexaboard SPI commands).
    int config_status;
    for (hx=0; hx<MAXHEXBDS; hx++) {
        if ((hexbd_mask & (1<<hx)) != 0) {

            // Verify that the command and response queues are empty.
            config_status = HEXBD_queue_status(hx);
            if ((config_status & 0x0101) != 0x0101) {
                fprintf(stderr,"hexbd: %d, queue error\n",(int)hx);
                exit(-1);
            }

            // Configure the hexaboard.
            fprintf(stderr,"Configuring hexbd %d...",(int)hx);
            config_status = configure_hexaboard(hx,0);
            config_status = configure_hexaboard(hx,1);
            fprintf(stderr,"done.\n");
            if (config_status < 0) {
                fprintf(stderr,"ERROR in configuration.\n");
                exit(-1);
            }
        }
    }

    // empty local fifo by forcing extra reads, ignore results
    fprintf(stderr,"Emptying local fifos (partially)...");
    for (hx=0; hx<8; hx++) HEXBD_read1000_local_fifo(hx,junk);
    for (hx=0; hx<8; hx++) HEXBD_read1000_local_fifo(hx,junk);
    for (hx=0; hx<8; hx++) HEXBD_read1000_local_fifo(hx,junk);
    fprintf(stderr,"done.\n");

    // Delay the start of "data taking" post configuration to 
    // stabilize the state of the chip
    fprintf(stderr,"Sleeping...");
    usleep(10000);
    sleep(1);
    fprintf(stderr,"done.\n");

    // Set the date stamp to non-zero, indicating we are done initializing.
    CTL_put_date_stamp0(0xAABB);
    CTL_put_date_stamp1(0x3434);
    date_stamp0 =  CTL_get_date_stamp0();
    date_stamp1 =  CTL_get_date_stamp1();
    fprintf(stderr,"date_stamp = 0x%04x 0x%04x\n",
            (int)date_stamp1, (int)date_stamp0);

    //===============================================================
    fprintf(stderr,"\nStart events acquisition\n");
    //===============================================================

    // reset trigger count
    CTL_reset_trig_count();

    // Agree on the size of the block of data that will set BLOCK_READY flag.
    CTL_put_block_size(30000);

    // Get the skiroc fifo block_size.
    block_size = CTL_get_block_size();
    fprintf(stderr,"block_size = %d\n", (int)block_size);

    // Get the skiroc mask, for debug...
    skiroc_mask1 = CTL_get_skiroc_mask1();
    skiroc_mask0 = CTL_get_skiroc_mask0();
    fprintf(stderr,"skiroc_mask = 0x%04x 0x%04x\n\n",
            (int)skiroc_mask1, (int)skiroc_mask0);

    uint64_t stamp0 = 0, stamp1 = 0, stamp2 = 0, trig0, trig1, old_trig0;
    uint64_t p_stamp = 0, f_stamp0 = 0, f_stamp1 = 0, f_stamp2 = 0;
    double trig_value, usec_value;
    int fifo_ready, block_ready, block_ready0, block_ready1, skiroc, j;
    int value0, value1;
    int raw_it;

    // Send a pulse back to the SYNC board. Give us a trigger.
    CTL_put_done();

    i = 0;
    int exit = 0;
    while(++i) {

        if(!((i - 1) % 10)) // every 10 events
            printf("Event %i\n", i);

        if( access( "stop.run.please", R_OK ) != -1 ) break;// exit if file is created

        // Get hexaboards ready.
        for(hexbd = 0; hexbd < MAXHEXBDS; hexbd++) {
            if((hexbd_mask & (1 << hexbd)) != 0) { 
                res = HEXBD_send_command(hexbd, CMD_RESETPULSE);
            }
        }

        usleep(HX_DELAY1);// Can be reduced to 1 MuS

        // Start acquisition.
        for(hexbd = 0; hexbd < MAXHEXBDS; hexbd++) {
            if((hexbd_mask & (1 << hexbd)) != 0) { 
                res = HEXBD_send_command(hexbd, CMD_SETSTARTACQ | 1);
                // usleep(HX_DELAY2);// Can be reduced to 1 MuS
                // res = HEXBD_send_command(hexbd, CMD_SETSTARTACQ); // CAN BE USED FOR SOFTWARE TRIGGER. NOT FOR REAL DATA!!!!!
            }// if hexbd_mask
        }// hexbd loop

        CTL_reset_fifos();

        // get the next trigger
        if(PED) {
            printf("sending triggers\n");
            // send put trigger to each ORM
            DATA_put_trigger_pulse(0);
            DATA_put_trigger_pulse(1);
            DATA_put_trigger_pulse(2);
            DATA_put_trigger_pulse(3);
        }
        else {

            // Send a pulse back to the SYNC board. Give us a trigger.
            old_trig0 = CTL_get_trig_count0();

            ////////////////////////Set Send_Trigger_OK to 1//////////////////////////////
            CTL_put_date_stamp0(1);
            /////////////////////////////////////////////////////////////////////////////

            printf("waiting for trigger\n");
            // Wait for trigger.
            trig0 = old_trig0;
            while (trig0 == old_trig0) {
                if( access( "stop.run.please", R_OK ) != -1 ) {exit=1;break;}// exit if file is created
                trig0 = CTL_get_trig_count0();
            }
            if(exit) break;
            printf("got a trigger\n");

            CTL_put_date_stamp0(0); // We have received a trigger, so its not OK to receive another one till readout is complete and SKIs are reset.

        }


        // tell hexbds to convert the data
        for(hexbd = 0; hexbd < MAXHEXBDS; hexbd++) {
            if((hexbd_mask & (1 << hexbd)) != 0) {
                res = HEXBD_send_command(hexbd, CMD_STARTCONPUL);
            }
        }

        usleep(HX_DELAY3);

        // tell hexbds to send the data back
        for(hexbd = 0; hexbd < MAXHEXBDS; hexbd++) {
            if((hexbd_mask & (1 << hexbd)) != 0) {
                res = HEXBD_send_command(hexbd, CMD_STARTROPUL);
            }
        }

        usleep(HX_DELAY4);

        isFifoEmpty = 0;
        while(!isFifoEmpty){
            isFifoEmpty = CTL_get_empty();
        }

    }// event loop

    end_spi();
    return(0);    

}// Main ends here

