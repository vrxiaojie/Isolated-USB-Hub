#include "e2prom.h"
#include "ui.h"
#include "Preferences.h"

eeprom_t eeprom;
/********************************** EEPROM断电保存配置 使用Preferences库********************************/
// 写入所有的UI设置。
// check: 写入前判断值是否修改 避免重复擦写
void EEPROM_write_ui_setting(bool check)
{
    Preferences prefs; // 声明Preferences对象
    prefs.begin("ui");
    for (uint8_t i = 0; i < UI_PARAM; i++)
    {
        switch (i)
        {
        case DISP_BRI:
            if (check && prefs.getUInt("DISP_BRI", ui.param[DISP_BRI]) != ui.param[DISP_BRI])
            {
                prefs.putUInt("DISP_BRI", ui.param[DISP_BRI]);
            }
            else
            {
                prefs.putUInt("DISP_BRI", ui.param[DISP_BRI]);
            }
        case TILE_ANI:
            if (check && prefs.getUInt("TILE_ANI", ui.param[TILE_ANI]) != ui.param[TILE_ANI])
            {
                prefs.putUInt("TILE_ANI", ui.param[TILE_ANI]);
            }
            else
            {
                prefs.putUInt("TILE_ANI", ui.param[TILE_ANI]);
            }
            break;
        case LIST_ANI:
            if (check && prefs.getUInt("LIST_ANI", ui.param[LIST_ANI]) != ui.param[LIST_ANI])
            {
                prefs.putUInt("LIST_ANI", ui.param[LIST_ANI]);
            }
            else
            {
                prefs.putUInt("LIST_ANI", ui.param[LIST_ANI]);
            }
            break;
        case WIN_ANI:
            if (check && prefs.getUInt("WIN_ANI", ui.param[WIN_ANI]) != ui.param[WIN_ANI])
            {
                prefs.putUInt("WIN_ANI", ui.param[WIN_ANI]);
            }
            else
            {
                prefs.putUInt("WIN_ANI", ui.param[WIN_ANI]);
            }
            break;
        case SPOT_ANI:
            if (check && prefs.getUInt("SPOT_ANI", ui.param[SPOT_ANI]) != ui.param[SPOT_ANI])
            {
                prefs.putUInt("SPOT_ANI", ui.param[SPOT_ANI]);
            }
            else
            {
                prefs.putUInt("SPOT_ANI", ui.param[SPOT_ANI]);
            }
            break;
        case TAG_ANI:
            if (check && prefs.getUInt("TAG_ANI", ui.param[TAG_ANI]) != ui.param[TAG_ANI])
            {
                prefs.putUInt("TAG_ANI", ui.param[TAG_ANI]);
            }
            else
            {
                prefs.putUInt("TAG_ANI", ui.param[TAG_ANI]);
            }
            break;
        case FADE_ANI:
            if (check && prefs.getUInt("FADE_ANI", ui.param[FADE_ANI]) != ui.param[FADE_ANI])
            {
                prefs.putUInt("FADE_ANI", ui.param[FADE_ANI]);
            }
            else
            {
                prefs.putUInt("FADE_ANI", ui.param[FADE_ANI]);
            }
            break;
        case BTN_SPT:
            if (check && prefs.getUInt("BTN_SPT", ui.param[BTN_SPT]) != ui.param[BTN_SPT])
            {
                prefs.putUInt("BTN_SPT", ui.param[BTN_SPT]);
            }
            else
            {
                prefs.putUInt("BTN_SPT", ui.param[BTN_SPT]);
            }
            break;
        case BTN_LPT:
            if (check && prefs.getUInt("BTN_LPT", ui.param[BTN_LPT]) != ui.param[BTN_LPT])
            {
                prefs.putUInt("BTN_LPT", ui.param[BTN_LPT]);
            }
            else
            {
                prefs.putUInt("BTN_LPT", ui.param[BTN_LPT]);
            }
            break;
        case TILE_UFD:
            if (check && prefs.getUInt("TILE_UFD", ui.param[TILE_UFD]) != ui.param[TILE_UFD])
            {
                prefs.putUInt("TILE_UFD", ui.param[TILE_UFD]);
            }
            else
            {
                prefs.putUInt("TILE_UFD", ui.param[TILE_UFD]);
            }
            break;
        case LIST_UFD:
            if (check && prefs.getUInt("LIST_UFD", ui.param[LIST_UFD]) != ui.param[LIST_UFD])
            {
                prefs.putUInt("LIST_UFD", ui.param[LIST_UFD]);
            }
            else
            {
                prefs.putUInt("LIST_UFD", ui.param[LIST_UFD]);
            }
            break;
        case TILE_LOOP:
            if (check && prefs.getUInt("TILE_LOOP", ui.param[TILE_LOOP]) != ui.param[TILE_LOOP])
            {
                prefs.putUInt("TILE_LOOP", ui.param[TILE_LOOP]);
            }
            else
            {
                prefs.putUInt("TILE_LOOP", ui.param[TILE_LOOP]);
            }
            break;
        case LIST_LOOP:
            if (check && prefs.getUInt("LIST_LOOP", ui.param[LIST_LOOP]) != ui.param[LIST_LOOP])
            {
                prefs.putUInt("LIST_LOOP", ui.param[LIST_LOOP]);
            }
            else
            {
                prefs.putUInt("LIST_LOOP", ui.param[LIST_LOOP]);
            }
            break;
        case WIN_BOK:
            if (check && prefs.getUInt("WIN_BOK", ui.param[WIN_BOK]) != ui.param[WIN_BOK])
            {
                prefs.putUInt("WIN_BOK", ui.param[WIN_BOK]);
            }
            else
            {
                prefs.putUInt("WIN_BOK", ui.param[WIN_BOK]);
            }
            break;
        }
    }
    prefs.end();
}

// 写入单个数据
void EEPROM_write_single_data(const char *part_name, const char *name, uint32_t value)
{
    Preferences prefs; // 声明Preferences对象
    prefs.begin(part_name);
    prefs.putUInt(name, value);
    prefs.end();
}

// 读取单个数据
uint32_t EEPROM_read_single_data(const char *part_name, const char *name)
{
    Preferences prefs; // 声明Preferences对象
    prefs.begin(part_name);
    uint32_t val = prefs.getUInt(name);
    prefs.end();
    return val;
}

void EEPROM_read_ui_setting()
{
    Preferences prefs; // 声明Preferences对象
    prefs.begin("ui");
    if (prefs.getBool("eeprom_init", false) == false) // 新设备没有初始化过，将会自动初始化并存入设置
    {
        Serial.println("设置未初始化");
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
            ui.param[DISP_BRI] = prefs.getUInt("DISP_BRI", ui.param[DISP_BRI]);
            break;
        case TILE_ANI:
            ui.param[TILE_ANI] = prefs.getUInt("TILE_ANI", ui.param[TILE_ANI]);
            break;
        case LIST_ANI:
            ui.param[LIST_ANI] = prefs.getUInt("LIST_ANI", ui.param[LIST_ANI]);
            break;
        case WIN_ANI:
            ui.param[WIN_ANI] = prefs.getUInt("WIN_ANI", ui.param[WIN_ANI]);
            break;
        case SPOT_ANI:
            ui.param[SPOT_ANI] = prefs.getUInt("SPOT_ANI", ui.param[SPOT_ANI]);
            break;
        case TAG_ANI:
            ui.param[TAG_ANI] = prefs.getUInt("TAG_ANI", ui.param[TAG_ANI]);
            break;
        case FADE_ANI:
            ui.param[FADE_ANI] = prefs.getUInt("FADE_ANI", ui.param[FADE_ANI]);
            break;
        case BTN_SPT:
            ui.param[BTN_SPT] = prefs.getUInt("BTN_SPT", ui.param[BTN_SPT]);
            break;
        case BTN_LPT:
            ui.param[BTN_LPT] = prefs.getUInt("BTN_LPT", ui.param[BTN_LPT]);
            break;
        case TILE_UFD:
            ui.param[TILE_UFD] = prefs.getUInt("TILE_UFD", ui.param[TILE_UFD]);
            break;
        case LIST_UFD:
            ui.param[LIST_UFD] = prefs.getUInt("LIST_UFD", ui.param[LIST_UFD]);
            break;
        case TILE_LOOP:
            ui.param[TILE_LOOP] = prefs.getUInt("TILE_LOOP", ui.param[TILE_LOOP]);
            break;
        case LIST_LOOP:
            ui.param[LIST_LOOP] = prefs.getUInt("LIST_LOOP", ui.param[LIST_LOOP]);
            break;
        case WIN_BOK:
            ui.param[WIN_BOK] = prefs.getUInt("WIN_BOK", ui.param[WIN_BOK]);
            break;
        default:
            break;
        }
    }
    prefs.end();
}

void EEPROM_clear_namespace(const char *ns)
{
    Preferences prefs; // 声明Preferences对象
    prefs.begin(ns);
    prefs.clear();
    prefs.end();
    Serial.printf("空间 %s 的所有键值对已清除 ", ns);
}
