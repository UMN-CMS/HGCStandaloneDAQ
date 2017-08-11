#include "uhal/uhal.hpp"

#include <vector>
#include <string>
#include <iostream>



int main(int argc, char *argv[]) {

    /*** initialization ***/
    uhal::setLogLevelTo(uhal::Error());// otherwise there's a bunch of crap printed out

    // iterators
    int i, j, k;
    unsigned int ui, uj, uk;

    std::vector<uhal::HwInterface> rdouts;


    /*** connection set-up ***/
    uhal::ConnectionManager manager("etc/connections.xml");
    std::vector<std::string> ids = manager.getDevices(".*rdout.*");// get all rdout boards
    for(auto id : ids)
        rdouts.push_back(manager.getDevice(id));

    // get connected cables (fifos)
    // loop over events
    // wait for block ready
    // read events
    // check fifo empty, if not then read til empty
    // send rdout_done
    // end event loop
    // closing actions
    // maybe use multiple threads?
    // edit makefile for C++11?

    return 0;
}
