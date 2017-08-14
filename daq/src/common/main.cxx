#include "uhal/uhal.hpp"
#include "daq.hpp"

#include <vector>
#include <string>
#include <cstdio>
#include <fstream>
#include <map>
#include <ctime>



int main(int argc, char *argv[]) {

    /*** argument processing ***/
    if(argc != 3) {
        fprintf(stderr, "Proper usage: ./bin/daq [RUN NUMBER] [NUMBER OF EVENTS]\n");
        exit(1);
    }
    const int RUNNUMBER = atoi(argv[1]);
    const int MAXEVENTS = atoi(argv[2]);
    printf("Beginning run number %i with %i events\n", RUNNUMBER, MAXEVENTS);


    /*** initialization ***/
    uhal::setLogLevelTo(uhal::Error());// otherwise there's a bunch of crap printed out

    // iterators
    int event, i;

    // containers
    std::vector<uhal::HwInterface> rdouts;
    std::map<std::string, std::ofstream*> file_map;// get file from the id of each HwInterface


    /*** set up connections ***/
    uhal::ConnectionManager manager("file://etc/connections.xml");
    std::vector<std::string> ids = manager.getDevices(".*rdout.*");// get all rdout boards
    for(auto id : ids)
        rdouts.push_back(manager.getDevice(id));

    
    /*** set up data files ***/
    
    // create time stamp
    std::time_t t = std::time(nullptr);
    char time_str[50];
    strftime(time_str, sizeof(time_str), "%d%m%Y_%H%M%S", std::localtime(&t));
  
    // make files for each readout board 
    i = 0;
    for(auto& rdout : rdouts) {
        char fname[50];
        sprintf(fname, "../data/RUN%i_%s_RDOUT%i.raw", RUNNUMBER, time_str, i);

        // pass pointer to map otherwise it won't work.
        // also have to use the HwInterface's id here because they don't like to be put in maps
        file_map[rdout.id()] = new std::ofstream(fname, std::ofstream::binary);

        i++;
    }


    /*** test connections ***/
    for(auto& rdout : rdouts) {
        uint32_t const0 = get_word(rdout, CONSTANT0);
        uint32_t const1 = get_word(rdout, CONSTANT1);

        if( (const0 != CONSTANT0_VAL) || (const1 != CONSTANT1_VAL) ) {
            fprintf(stderr, "Constants do not match. Expected ");
            fprintf(stderr, "%8x %8x, got ", CONSTANT0_VAL, CONSTANT1_VAL);
            fprintf(stderr, "%8x %8x instead.\n", const0, const1);
            exit(1);
        }
    }


    /*** main event loop ***/
    for(event = 0; event < MAXEVENTS; event++) {
        for(auto& rdout : rdouts) {

            // wait for block ready
            while(!get_word(rdout, BLOCK_READY)) {
                break;// TODO DELETE THIS LINE WHEN THINGS ACTUALLY WORK!
                continue;
            }

            // get the event data
            std::vector<uint32_t> ev_data = get_nwords(rdout, FIFO, BLOCKSIZE);

            // TODO check the data
            if(!verify_data(ev_data)) {
                fprintf(stderr, "Data verification failed. Stopping run.\n");
                exit(1);
            }

            // write the event data to a separate file for each rdout board
            write_data(*file_map[rdout.id()], ev_data);

            // check if the fifo is empty, if not then read til it is empty
            while(!get_word(rdout, EMPTY)) {
                break;// TODO DELETE THIS LINE WHEN THINGS ACTUALLY WORK!
                (void) get_word(rdout, FIFO);
            }

            // send rdout done magic
            put_word(rdout, RDOUT_DONE, RDOUT_DONE_MAGIC);

        }// end loop over rdouts
    }// end loop over events


    /*** closing ***/

    // close files
    for(auto& rdout : rdouts) {
        file_map[rdout.id()]->close();
    }

    // TODO maybe use multiple threads?

    return 0;
}