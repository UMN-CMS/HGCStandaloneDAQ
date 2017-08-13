#include "uhal/uhal.hpp"
#include "daq.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <fstream>

#define MAXEVENTS 100



int main(int argc, char *argv[]) {

    /*** initialization ***/
    uhal::setLogLevelTo(uhal::Error());// otherwise there's a bunch of crap printed out

    int event, i;
    std::vector<uhal::HwInterface> rdouts;
    std::map<uhal::HwInterface, std::ofstream> file_map;


    /*** set up connections ***/
    uhal::ConnectionManager manager("file://etc/connections.xml");
    std::vector<std::string> ids = manager.getDevices(".*rdout.*");// get all rdout boards
    for(auto id : ids)
        rdouts.push_back(manager.getDevice(id));


    /*** set up data files ***/
    for(auto& rdout : rdouts) {
        std::string fname = std::string("test_RDOUT") + std::string(i) + std::string(".raw");
        std::cout << fname << std::endl;
        // std::ofstream fout(fname, std::ofstream::binary);
        // file_map.insert(std::make_pair(rdout, fout));
        i++;
    }


    /*** test connections ***/
    for(auto& rdout : rdouts) {
        uint32_t const0 = get_word(rdout, CONSTANT0);
        uint32_t const1 = get_word(rdout, CONSTANT1);

        if( (const0 != CONSTANT0_VAL) || (const1 != CONSTANT1_VAL) ) {
            std::cout << "Constants do not match. Expected ";
            std::cout << std::hex << CONSTANT0_VAL << " " << CONSTANT1_VAL << ", got ";
            std::cout << std::hex << const0 << " " << const1 << " instead.\n";
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

            // TODO write the event data to a separate file for each rdout board - use a map?
            // write_data(fout, ev_data);

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
    fout.close();

    // maybe use multiple threads?

    return 0;
}
