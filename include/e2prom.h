#ifndef E2PROM_H_
#define E2PROM_H_
#include <Arduino.h>
// EEPROM变量
struct eeprom_t
{
    bool init;
    bool change;
    int address;
    uint8_t check;
};
extern eeprom_t eeprom;

void EEPROM_write_ui_setting(bool check);
void EEPROM_read_ui_setting();
void EEPROM_write_single_data(const char *part_name, const char *name, uint32_t value);
uint32_t EEPROM_read_single_data(const char *part_name, const char *name);
void EEPROM_clear_namespace(const char *ns);
void EEPROM_read_wifi_setting();
void EEPROM_write_wifi_setting();
void EEPROM_write_activation();
bool EEPROM_read_activation();
#endif