#ifndef DAQ_HPP
#define DAQ_HPP

#include "uhal/uhal.hpp"
#include "ipbus_utils.hpp"
#include "rdout.hpp"
#include "dataformat.hpp"

#include <fstream>

void write_data(std::ofstream& fout, std::vector<uint32_t> data) {
    fout.write(reinterpret_cast<char*>(&data[0]), data.size() * sizeof(data[0]));
    fout.flush();
}

#endif
