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

/**
 * @brief 将UI设置写入EEPROM
 * @param check 是否进行检查
 */
void EEPROM_write_ui_setting(bool check);

/**
 * @brief 从EEPROM读取UI设置
 */
void EEPROM_read_ui_setting();

/**
 * @brief 在指定的命名空间中写入单个数据
 * @param part_name 命名空间名称
 * @param name 数据键名
 * @param value 要写入的32位无符号整数值
 */
void EEPROM_write_single_data(const char *part_name, const char *name, uint32_t value);

/**
 * @brief 从指定的命名空间中读取单个数据
 * @param part_name 命名空间名称
 * @param name 数据键名
 * @return uint32_t 读取到的32位无符号整数值
 */
uint32_t EEPROM_read_single_data(const char *part_name, const char *name);

/**
 * @brief 清除指定的命名空间
 * @param ns 要清除的命名空间名称
 */
void EEPROM_clear_namespace(const char *ns);

/**
 * @brief 从EEPROM读取WiFi设置
 */
void EEPROM_read_wifi_setting();

/**
 * @brief 将WiFi设置写入EEPROM
 */
void EEPROM_write_wifi_setting();

/**
 * @brief 写入激活状态到EEPROM
 */
void EEPROM_write_activation();

/**
 * @brief 从EEPROM读取激活状态
 * @return true 如果已激活
 * @return false 如果未激活
 */
bool EEPROM_read_activation();

/**
 * @brief 将监视器设置写入EEPROM
 */
void EEPROM_write_monitor_setting();

/**
 * @brief 从EEPROM读取监视器设置
 */
void EEPROM_read_monitor_setting();

/**
 * @brief 写入OTA标志到EEPROM
 * @param flag 要写入的布尔值标志
 */
void EEPROM_write_ota_flag(bool flag);

/**
 * @brief 从EEPROM读取OTA标志
 * @return true 如果OTA标志被设置
 * @return false 如果OTA标志未被设置
 */
bool EEPROM_read_ota_flag();
#endif