#include "ina226_data.h"

INA226_DATA_t ina226_data[4];

INA226 ina226_ctrl[4] = {
    INA226(0x45),
    INA226(0x44),
    INA226(0x41),
    INA226(0x40)};