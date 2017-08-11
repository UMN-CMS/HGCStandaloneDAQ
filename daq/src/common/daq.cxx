#include "uhal/uhal.hpp"
#include "daq.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <cstdio>



int main(int argc, char *argv[]) {

    /*** initialization ***/
    uhal::setLogLevelTo(uhal::Error());// otherwise there's a bunch of crap printed out

    // iterators
    int i, j, k;
    unsigned int ui, uj, uk;

    std::vector<uhal::HwInterface> rdouts;
    std::vector<std::vector<std::string> > active_fifos;


    /*** set up connections ***/
    uhal::ConnectionManager manager("file://etc/connections.xml");
    std::vector<std::string> ids = manager.getDevices(".*rdout.*");// get all rdout boards
    for(auto id : ids)
        rdouts.push_back(manager.getDevice(id));


    /*** test connections ***/
    for(auto& rdout : rdouts) {
        uint32_t const0 = get_word(rdout, CONST0);
        uint32_t const1 = get_word(rdout, CONST1);

        if( !(const0 != CONST0_VAL) || (const1 != CONST1_VAL) ) {
            std::cout << "Constants do not match. Expected ";
            std::cout << std::hex << CONST0_VAL << " " << CONST1_VAL << ", got ";
            std::cout << std::hex << const0 << " " << const1 << " instead.\n";
            exit(1);
        }          
    }


    // get connected fifos
    // loop over events
    // wait for block ready
    // read events
    // check fifo empty, if not then read til empty
    // send rdout_done
    // end event loop
    // closing actions
    // maybe use multiple threads?

    return 0;
}
