#ifndef E2PROM_H_
#define E2PROM_H_
#include <Arduino.h>
// EEPROM变量
#define EEPROM_CHECK 11
struct eeprom_t
{
    bool init;
    bool change;
    int address;
    uint8_t check;
};
extern eeprom_t eeprom;

void eeprom_write_modified_data();
void eeprom_write_all_data();
void eeprom_read_all_data();
void eeprom_init();
#endif