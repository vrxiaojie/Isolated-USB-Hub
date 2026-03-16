#include "e2prom.h"
#include "ui.h"
#include "Wifi_Config.h"
#include "Preferences.h"

eeprom_t eeprom;
/********************************** EEPROM断电保存配置 使用Preferences库********************************/
// 写入所有的UI设置。
// check: 写入前判断值是否修改 避免重复擦写
void EEPROM_write_ui_setting(bool check)
{
    Preferences prefs;
    prefs.begin("ui");
    for (uint8_t i = 0; i < UI_PARAM; i++)
    {
        switch (i)
        {
        case DISP_BRI:
            if (prefs.getUChar("DISP_BRI", ui.param[DISP_BRI]) != ui.param[DISP_BRI])
            {
                prefs.putUChar("DISP_BRI", ui.param[DISP_BRI]);
            }
        case TILE_ANI:
            if (prefs.getUChar("TILE_ANI", ui.param[TILE_ANI]) != ui.param[TILE_ANI])
            {
                prefs.putUChar("TILE_ANI", ui.param[TILE_ANI]);
            }
            break;
        case LIST_ANI:
            if (prefs.getUChar("LIST_ANI", ui.param[LIST_ANI]) != ui.param[LIST_ANI])
            {
                prefs.putUChar("LIST_ANI", ui.param[LIST_ANI]);
            }
            break;
        case WIN_ANI:
            if (prefs.getUChar("WIN_ANI", ui.param[WIN_ANI]) != ui.param[WIN_ANI])
            {
                prefs.putUChar("WIN_ANI", ui.param[WIN_ANI]);
            }
            break;
        case SPOT_ANI:
            if (prefs.getUChar("SPOT_ANI", ui.param[SPOT_ANI]) != ui.param[SPOT_ANI])
            {
                prefs.putUChar("SPOT_ANI", ui.param[SPOT_ANI]);
            }
            break;
        case TAG_ANI:
            if (prefs.getUChar("TAG_ANI", ui.param[TAG_ANI]) != ui.param[TAG_ANI])
            {
                prefs.putUChar("TAG_ANI", ui.param[TAG_ANI]);
            }
            break;
        case FADE_ANI:
            if (prefs.getUChar("FADE_ANI", ui.param[FADE_ANI]) != ui.param[FADE_ANI])
            {
                prefs.putUChar("FADE_ANI", ui.param[FADE_ANI]);
            }
            break;
        case BTN_SPT:
            if (prefs.getUChar("BTN_SPT", ui.param[BTN_SPT]) != ui.param[BTN_SPT])
            {
                prefs.putUChar("BTN_SPT", ui.param[BTN_SPT]);
            }
            break;
        case BTN_LPT:
            if (prefs.getUChar("BTN_LPT", ui.param[BTN_LPT]) != ui.param[BTN_LPT])
            {
                prefs.putUChar("BTN_LPT", ui.param[BTN_LPT]);
            }
            break;
        case TILE_UFD:
            if (prefs.getUChar("TILE_UFD", ui.param[TILE_UFD]) != ui.param[TILE_UFD])
            {
                prefs.putUChar("TILE_UFD", ui.param[TILE_UFD]);
            }
            break;
        case LIST_UFD:
            if (prefs.getUChar("LIST_UFD", ui.param[LIST_UFD]) != ui.param[LIST_UFD])
            {
                prefs.putUChar("LIST_UFD", ui.param[LIST_UFD]);
            }
            break;
        case TILE_LOOP:
            if (prefs.getUChar("TILE_LOOP", ui.param[TILE_LOOP]) != ui.param[TILE_LOOP])
            {
                prefs.putUChar("TILE_LOOP", ui.param[TILE_LOOP]);
            }
            break;
        case LIST_LOOP:
            if (prefs.getUChar("LIST_LOOP", ui.param[LIST_LOOP]) != ui.param[LIST_LOOP])
            {
                prefs.putUChar("LIST_LOOP", ui.param[LIST_LOOP]);
            }
            break;
        case WIN_BOK:
            if (prefs.getUChar("WIN_BOK", ui.param[WIN_BOK]) != ui.param[WIN_BOK])
            {
                prefs.putUChar("WIN_BOK", ui.param[WIN_BOK]);
            }
            break;
        }
    }
    prefs.end();
}

// 写入单个数据
void EEPROM_write_single_data(const char *part_name, const char *name, uint32_t value)
{
    Preferences prefs;
    prefs.begin(part_name);
    prefs.putUChar(name, value);
    prefs.end();
}

// 读取单个数据
uint32_t EEPROM_read_single_data(const char *part_name, const char *name)
{
    Preferences prefs;
    prefs.begin(part_name);
    uint32_t val = prefs.getUChar(name);
    prefs.end();
    return val;
}

void EEPROM_read_ui_setting()
{
    Preferences prefs;
    prefs.begin("ui");
    if (prefs.getBool("eeprom_init", false) == false) // 新设备没有初始化过，将会自动初始化并存入设置
    {
        // Serial.println("设置未初始化");
        prefs.putBool("eeprom_init", true);
        ui_param_init();
        EEPROM_write_ui_setting(false);
        return;
    }

    for (uint8_t i = 0; i < UI_PARAM; i++)
    {
        switch (i)
        {
        case DISP_BRI:
            ui.param[DISP_BRI] = prefs.getUChar("DISP_BRI", ui.param[DISP_BRI]);
            break;
        case TILE_ANI:
            ui.param[TILE_ANI] = prefs.getUChar("TILE_ANI", ui.param[TILE_ANI]);
            break;
        case LIST_ANI:
            ui.param[LIST_ANI] = prefs.getUChar("LIST_ANI", ui.param[LIST_ANI]);
            break;
        case WIN_ANI:
            ui.param[WIN_ANI] = prefs.getUChar("WIN_ANI", ui.param[WIN_ANI]);
            break;
        case SPOT_ANI:
            ui.param[SPOT_ANI] = prefs.getUChar("SPOT_ANI", ui.param[SPOT_ANI]);
            break;
        case TAG_ANI:
            ui.param[TAG_ANI] = prefs.getUChar("TAG_ANI", ui.param[TAG_ANI]);
            break;
        case FADE_ANI:
            ui.param[FADE_ANI] = prefs.getUChar("FADE_ANI", ui.param[FADE_ANI]);
            break;
        case BTN_SPT:
            ui.param[BTN_SPT] = prefs.getUChar("BTN_SPT", ui.param[BTN_SPT]);
            break;
        case BTN_LPT:
            ui.param[BTN_LPT] = prefs.getUChar("BTN_LPT", ui.param[BTN_LPT]);
            break;
        case TILE_UFD:
            ui.param[TILE_UFD] = prefs.getUChar("TILE_UFD", ui.param[TILE_UFD]);
            break;
        case LIST_UFD:
            ui.param[LIST_UFD] = prefs.getUChar("LIST_UFD", ui.param[LIST_UFD]);
            break;
        case TILE_LOOP:
            ui.param[TILE_LOOP] = prefs.getUChar("TILE_LOOP", ui.param[TILE_LOOP]);
            break;
        case LIST_LOOP:
            ui.param[LIST_LOOP] = prefs.getUChar("LIST_LOOP", ui.param[LIST_LOOP]);
            break;
        case WIN_BOK:
            ui.param[WIN_BOK] = prefs.getUChar("WIN_BOK", ui.param[WIN_BOK]);
            break;
        default:
            break;
        }
    }
    prefs.end();
}

void EEPROM_write_wifi_setting()
{
    Preferences prefs;
    prefs.begin("WIFI");
    for (uint8_t i = 0; i < WIFI_PARAM; i++)
    {
        switch (i)
        {
        case WIFI_AUTO_CONN:
            if (prefs.getUChar("WIFI_AUTO_CONN") != wifi.param[WIFI_AUTO_CONN])
            {
                prefs.putUChar("WIFI_AUTO_CONN", wifi.param[WIFI_AUTO_CONN]);
            }
            break;
        case WIFI_DISABLE_ON_SLEEP:
            if (prefs.getUChar("WL_SLEEP") != wifi.param[WIFI_DISABLE_ON_SLEEP])
            {
                prefs.putUChar("WL_SLEEP", wifi.param[WIFI_DISABLE_ON_SLEEP]);
            }
            break;
        }
    }
    prefs.end();
}

void EEPROM_read_wifi_setting()
{
    Preferences prefs;
    prefs.begin("WIFI");
    for (uint8_t i = 0; i < WIFI_PARAM; i++)
    {
        switch (i)
        {
        case WIFI_AUTO_CONN:
            wifi.param[WIFI_AUTO_CONN] = prefs.getUChar("WIFI_AUTO_CONN");
            break;
        case WIFI_DISABLE_ON_SLEEP:
            wifi.param[WIFI_DISABLE_ON_SLEEP] = prefs.getUChar("WL_SLEEP");
            break;
        }
    }
}

// 获取激活状态
bool EEPROM_read_activation()
{
    Preferences prefs;
    prefs.begin("activation");
    return prefs.getBool("activated", false);
}

// 保存激活结果
void EEPROM_write_activation()
{
    Preferences prefs;
    prefs.begin("activation");
    prefs.putBool("activated", true);
    prefs.end();
}

void EEPROM_write_monitor_setting()
{
    Preferences prefs;
    prefs.begin("usb_monitor");
    for (uint8_t i = 0; i < USB_MONITOR_PARAM; i++)
    {
        switch (i)
        {
        case REFRESH_RATE:
            if (prefs.getUChar("REFRESH_RATE") != usb_monitor.param[REFRESH_RATE])
            {
                prefs.putUChar("REFRESH_RATE", usb_monitor.param[REFRESH_RATE]);
            }
            break;
        case MONITOR_SERIAL_OUTPUT:
            if (prefs.getUChar("SERIAL_OUTPUT") != usb_monitor.param[MONITOR_SERIAL_OUTPUT])
            {
                prefs.putUChar("SERIAL_OUTPUT", usb_monitor.param[MONITOR_SERIAL_OUTPUT]);
            }
            break;
        }
    }
    prefs.end();
}

void EEPROM_read_monitor_setting()
{
    Preferences prefs;
    prefs.begin("usb_monitor");
    for (uint8_t i = 0; i < USB_MONITOR_PARAM; i++)
    {
        switch (i)
        {
        case REFRESH_RATE:
            usb_monitor.param[REFRESH_RATE] = prefs.getUChar("REFRESH_RATE", 10);
            break;
        case MONITOR_SERIAL_OUTPUT:
            usb_monitor.param[MONITOR_SERIAL_OUTPUT] = prefs.getUChar("SERIAL_OUTPUT", 0);
            break;
        }
    }
}

void EEPROM_write_ota_flag(bool flag)
{
    Preferences prefs;
    prefs.begin("ota");
    prefs.putBool("ota_ready", flag);
    prefs.end();
}

bool EEPROM_read_ota_flag()
{
    Preferences prefs;
    prefs.begin("ota");
    return prefs.getBool("ota_ready", false);
}

void EEPROM_clear_namespace(const char *ns)
{
    Preferences prefs;
    prefs.begin(ns);
    prefs.clear();
    prefs.end();
    // Serial.printf("空间 %s 的所有键值对已清除 ", ns);
}
