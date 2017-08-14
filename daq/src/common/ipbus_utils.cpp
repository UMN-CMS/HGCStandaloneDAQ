#ifndef IPBUS_UTILS_CPP
#define IPBUS_UTILS_CPP

#include "ipbus_utils.hpp"


/*** reading ***/
// reads a single word from a node
uint32_t get_word(uhal::HwInterface& hw, std::string node) {
    uhal::ValWord<uint32_t> val = hw.getNode(node).read();
    hw.dispatch();
    return val.value();
}

// reads blocksize words from a node
std::vector<uint32_t> get_nwords(uhal::HwInterface& hw, std::string node, int blocksize) {
    uhal::ValVector<uint32_t> vals = hw.getNode(node).readBlock(blocksize);
    hw.dispatch();
    return vals.value();
}


/*** writing ***/
// writes a single word to a node
void put_word(uhal::HwInterface& hw, std::string node, uint32_t val) {
    hw.getNode(node).write(val);
    hw.dispatch();
}

// writes a vector of words to a node
void put_nwords(uhal::HwInterface& hw, std::string node, std::vector<uint32_t> vals) {
    hw.getNode(node).writeBlock(vals);
    hw.dispatch();
}

#endif
