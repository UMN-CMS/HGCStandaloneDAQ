#ifndef RDOUT_HPP
#define RDOUT_HPP

#include <string>


/*** nodes ***/
const std::string FIRMWARE_VERSION =    "FIRMWARE_VERSION";
const std::string CONSTANT0 =           "CONSTANT0";
const std::string CONSTANT1 =           "CONSTANT1";
const std::string DISABLE_DEBUG =       "DISABLE_DEBUG";
const std::string SKIROC_MASK =         "SKIROC_MASK";
const std::string DATE_STAMP =          "DATE_STAMP";
const std::string BLOCK_SIZE =          "BLOCK_SIZE";
const std::string PEDESTAL_MODE =       "PEDESTAL_MODE";
const std::string TRIG_COUNT =          "TRIG_COUNT";
const std::string CLK_COUNT0 =          "CLK_COUNT0";
const std::string CLK_COUNT1 =          "CLK_COUNT1";
const std::string BLOCK_READY =         "BLOCK_READY";
const std::string ALMOST_FULL =         "ALMOST_FULL";
const std::string EMPTY =               "EMPTY";
const std::string FIFO =                "FIFO";
const std::string FULL =                "FULL";
const std::string OCCUPANCY =           "OCCUPANCY";
const std::string RDOUT_DONE =          "RDOUT_DONE";


/*** values ***/
const uint32_t CONSTANT0_VAL =      0xdead;
const uint32_t CONSTANT1_VAL =      0xbeef;
const uint32_t RDOUT_DONE_MAGIC =   0xABCD4321;

#endif
