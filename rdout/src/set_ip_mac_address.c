// gcc -O set_ip_mac_address.c ctl_orm.c spi_common.c -l bcm2835 -o set_ip_mac_address

#include "spi_common.h"
#include "ctl_orm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    // make sure args are correct
    if(argc != 3) {
        printf("Incorrect arguments. Should be [IP ADDR] [MAC ADDR]\n");
        exit(1);
    }

    // get ip and mac from args
    char *ip_str = argv[1];
    char *mac_str = argv[2];

    // delimiters and empty ptr
    char *ip_delim = ".";
    char *mac_delim = ":";
    char *token;

    // split IP into 4 ints
    int ip_b0, ip_b1, ip_b2, ip_b3;
    token = strtok(ip_str, ip_delim);
    ip_b0 = atoi(token);
    token = strtok(NULL, ip_delim);
    ip_b1 = atoi(token);
    token = strtok(NULL, ip_delim);
    ip_b2 = atoi(token);
    token = strtok(NULL, ip_delim);
    ip_b3 = atoi(token);

    // split MAC into 6 ints
    int mac_b0, mac_b1, mac_b2, mac_b3, mac_b4, mac_b5;
    token = strtok(mac_str, mac_delim);
    mac_b0 = (int) strtol(token, NULL, 16);
    token = strtok(NULL, mac_delim);
    mac_b1 = (int) strtol(token, NULL, 16);
    token = strtok(NULL, mac_delim);
    mac_b2 = (int) strtol(token, NULL, 16);
    token = strtok(NULL, mac_delim);
    mac_b3 = (int) strtol(token, NULL, 16);
    token = strtok(NULL, mac_delim);
    mac_b4 = (int) strtol(token, NULL, 16);
    token = strtok(NULL, mac_delim);
    mac_b5 = (int) strtol(token, NULL, 16);

    // combine IP into 2 hextets (16-bit ints)
    int ip_h0, ip_h1;
    ip_h0 = (ip_b2 << 8) | ip_b3;
    ip_h1 = (ip_b0 << 8) | ip_b1;

    // combine MAC into 3 hextets
    int mac_h0, mac_h1, mac_h2;
    mac_h0 = (mac_b4 << 8) | mac_b5;
    mac_h1 = (mac_b2 << 8) | mac_b3;
    mac_h2 = (mac_b0 << 8) | mac_b1;

    // initialize spi bus
    init_spi();

    // print old addresses
    if(0) {
        int ip0, ip1;
        ip0 = CTL_get_ip_address0();
        ip1 = CTL_get_ip_address1();
        printf("old IP0 Address: %x\n", ip0);
        printf("old IP1 Address: %x\n", ip1);

        int mac0, mac1, mac2;
        mac0 = CTL_get_mac_address0();
        mac1 = CTL_get_mac_address1();
        mac2 = CTL_get_mac_address2();
        printf("old MAC0 Address: %x\n", mac0);
        printf("old MAC1 Address: %x\n", mac1);
        printf("old MAC2 Address: %x\n", mac2);
    }

    // set IP
    CTL_put_ip_address0(ip_h0);
    CTL_put_ip_address1(ip_h1);

    // set MAC
    CTL_put_mac_address0(mac_h0);
    CTL_put_mac_address1(mac_h1);
    CTL_put_mac_address2(mac_h2);

    // print new addresses
    if(0) {
        int ip0, ip1;
        ip0 = CTL_get_ip_address0();
        ip1 = CTL_get_ip_address1();
        printf("new IP0 Address: %x\n", ip0);
        printf("new IP1 Address: %x\n", ip1);

        int mac0, mac1, mac2;
        mac0 = CTL_get_mac_address0();
        mac1 = CTL_get_mac_address1();
        mac2 = CTL_get_mac_address2();
        printf("new MAC0 Address: %x\n", mac0);
        printf("new MAC1 Address: %x\n", mac1);
        printf("new MAC2 Address: %x\n", mac2);
    }

    // printf("resetting the si5324\n");
    spi_put_16bits(4,0x1,0xF); // Reset the si5324 and then ipbus.
    spi_put_16bits(4,0x1,0x0); // Reset the si5324 and then ipbus.

    // close spi bus
    end_spi();

    return 0;
}

