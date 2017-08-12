#ifndef IPBUS_UTILS_HPP
#define IPBUS_UTILS_HPP

#include "uhal/uhal.hpp"

#include <vector>
#include <string>


/*** reading ***/
uint32_t get_word(uhal::HwInterface& hw, std::string node) {
    uhal::ValWord<uint32_t> val = hw.getNode(node).read();
    hw.dispatch();
    return val.value();
}

std::vector<uint32_t> get_nwords(uhal::HwInterface& hw, std::string node, int blocksize) {
    uhal::ValVector<uint32_t> vals = hw.getNode(node).readBlock(blocksize);
    hw.dispatch();
    return vals.value();
}


/*** writing ***/
void put_word(uhal::HwInterface& hw, std::string node, uint32_t val) {
    hw.getNode(node).write(val);
    hw.dispatch();
}

void put_nwords(uhal::HwInterface& hw, std::string node, std::vector<uint32_t> vals) {
    hw.getNode(node).writeBlock(vals);
    hw.dispatch();
}

#endif
