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
    for(ui = 0; ui < ids.size(); ui++)
        rdouts.push_back(manager.getDevice(ids[ui]));

    return 0;
}
