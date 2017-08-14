#ifndef DAQ_HPP
#define DAQ_HPP

#include "uhal/uhal.hpp"
#include "ipbus_utils.hpp"
#include "rdout.hpp"
#include "dataformat.hpp"

#include <fstream>

// writes a vector of uint32_t's to an ofstream
void write_data(std::ofstream&, std::vector<uint32_t>);

// verifies the data by checking CRC, etc...
bool verify_data(std::vector<uint32_t>);

#endif
