#include "e2prom.h"
#include "ui.h"
/************************************* 断电保存 *************************************/

#include <EEPROM.h>

eeprom_t eeprom;
uint8_t eeprom_check_param[EEPROM_CHECK] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k'};

// EEPROM写数据
void eeprom_write_all_data()
{
    eeprom.address = 0;
    for (uint8_t i = 0; i < EEPROM_CHECK; ++i)
        EEPROM.write(eeprom.address + i, eeprom_check_param[i]);
    eeprom.address += EEPROM_CHECK;
    for (uint8_t i = 0; i < UI_PARAM; ++i)
        EEPROM.write(eeprom.address + i, ui.param[i]);
    EEPROM.commit();
    eeprom.address += UI_PARAM;
}

// EEPROM读数据，开机初始化时执行一遍
void eeprom_read_all_data()
{
    eeprom.address = EEPROM_CHECK;
    for (uint8_t i = 0; i < UI_PARAM; ++i)
        ui.param[i] = EEPROM.read(eeprom.address + i);
    eeprom.address += UI_PARAM;
}

// 开机检查是否已经修改过，没修改过则跳过读配置步骤，用默认设置
void eeprom_init()
{
    eeprom.check = 0;
    eeprom.address = 0;
    EEPROM.begin(EEPROM_CHECK + UI_PARAM);
    for (uint8_t i = 0; i < EEPROM_CHECK; ++i)
        if (EEPROM.read(eeprom.address + i) != eeprom_check_param[i])
            eeprom.check++;
    if (eeprom.check <= 1)
        eeprom_read_all_data(); // 允许一位误码
    else
        ui_param_init();
}