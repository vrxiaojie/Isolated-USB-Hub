#ifndef INA226_DATA_H_
#define INA226_DATA_H_
#include <INA226.h>

#define SDA 5
#define SCL 4

typedef struct INA226_DATA
{
    float busVoltage;
    float current_mA;
    float power_mW;
    bool init;
} INA226_DATA_t;

extern INA226_DATA_t ina226_data[4];

extern INA226 ina226_ctrl[4];

#endif
