#ifndef IPBUS_UTILS_HPP
#define IPBUS_UTILS_HPP

#include "uhal/uhal.hpp"

#include <vector>
#include <string>


/*** reading ***/
// reads a single word from a node
uint32_t get_word(uhal::HwInterface& hw, std::string node);

// reads blocksize words from a node
std::vector<uint32_t> get_nwords(uhal::HwInterface& hw, std::string node, int blocksize);


/*** writing ***/
// writes a single word to a node
void put_word(uhal::HwInterface& hw, std::string node, uint32_t val);

// writes a vector of words to a node
void put_nwords(uhal::HwInterface& hw, std::string node, std::vector<uint32_t> vals);

#endif
