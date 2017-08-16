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
    if(argc != 5) {
        fprintf(stderr, "Proper usage: ./bin/main [RUN NUMBER] [NUMBER OF EVENTS] [PED] [DATA DIRECTORY]\n");
        exit(1);
    }
    const int RUNNUMBER = atoi(argv[1]);
    const int MAXEVENTS = atoi(argv[2]);
    const bool PED = atoi(argv[3]);
    const std::string DATADIR = argv[4];
    if(PED) printf("Beginning PED run number %i with %i events.\n", RUNNUMBER, MAXEVENTS);
    else printf("Beginning run number %i with %i events.\n", RUNNUMBER, MAXEVENTS);


    /*** initialization ***/
    uhal::setLogLevelTo(uhal::Error());// otherwise there's a bunch of crap printed out

    // iterators
    int event;

    // containers
    std::vector<uhal::HwInterface> rdouts;


    /*** set up connections ***/
    uhal::ConnectionManager manager("file://etc/connections.xml");
    std::vector<std::string> ids = manager.getDevices(".*rdout.*");// get all rdout boards
    for(auto id : ids)
        rdouts.push_back(manager.getDevice(id));


    /*** set up data file ***/
    
    // create time stamp
    std::time_t t = std::time(nullptr);
    char time_str[50];
    strftime(time_str, sizeof(time_str), "%d%m%Y_%H%M%S", std::localtime(&t));
  
    // create filename
    char fname[50];
    if(PED) sprintf(fname, "%s/PED_RUN%i_%s.raw", DATADIR.c_str(), RUNNUMBER, time_str);
    else sprintf(fname, "%s/RUN%i_%s.raw", DATADIR.c_str(), RUNNUMBER, time_str);

    // open file
    std::ofstream fout(fname, std::ofstream::binary);



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
            write_data(fout, ev_data);

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
    fout.close();

    // TODO maybe use multiple threads?

    return 0;
}
