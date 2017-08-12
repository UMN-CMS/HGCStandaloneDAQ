#include "uhal/uhal.hpp"
#include "daq.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <cstdio>

#define MAXEVENTS 100

void print_ev_data(int event, std::vector<uint32_t> data) {
    std::cout << "EVENT " << event << std::endl;
    for(auto dat : data) std::cout << std::hex << dat << std::endl;
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {

    /*** initialization ***/
    uhal::setLogLevelTo(uhal::Error());// otherwise there's a bunch of crap printed out

    // iterators
    int event;

    std::vector<uhal::HwInterface> rdouts;
    std::vector<std::vector<std::string> > active_fifos;


    /*** set up connections ***/
    uhal::ConnectionManager manager("file://etc/connections.xml");
    std::vector<std::string> ids = manager.getDevices(".*rdout.*");// get all rdout boards
    for(auto id : ids)
        rdouts.push_back(manager.getDevice(id));


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
    }// end loop over rdouts


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

            // TODO write the event data

            // check if the fifo is empty, if not then read til it is empty
            while(!get_word(rdout, EMPTY)) {
                break;// TODO DELETE THIS LINE WHEN THINGS ACTUALLY WORK!
                (void) get_word(rdout, FIFO);
            }

            // send rdout done magic
            put_word(rdout, RDOUT_DONE, RDOUT_DONE_MAGIC);

        }// end loop over rdouts
    }// end loop over events


    // closing actions
    // maybe use multiple threads?

    return 0;
}
