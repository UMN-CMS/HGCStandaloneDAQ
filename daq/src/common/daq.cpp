#ifndef DAQ_CPP
#define DAQ_CPP

#include "daq.hpp"


// writes a vector of uint32_t's to an ofstream
void write_data(std::ofstream& fout, std::vector<uint32_t> data) {
    fout.write(reinterpret_cast<char*>(&data[0]), data.size() * sizeof(data[0]));
    fout.flush();
}


// verifies the data by checking CRC, etc...
bool verify_data(std::vector<uint32_t> data) {
    // TODO
    return true;
}

#endif
