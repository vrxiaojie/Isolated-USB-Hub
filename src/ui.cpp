#include "ui.h"
#include <U8g2lib.h>
#include "knob.h"
#include "e2prom.h"
#include "RTOS_Task.h"
#include "ina226_data.h"
#include "pic.h"
#include "oled.h"
#include "switch.h"
#include "Wifi_Config.h"

/************************************* 定义内容 *************************************/

/************************************* 文字内容 *************************************/
// 大标题
M_SELECT main_menu[]{
    {"Sleep"},
    {"Switch"},
    {"Monitor"},
    {"WiFi"},
    {"Setting"},
    {"OTA"},
};
// 小标题
M_SELECT main_menu_exp[]{
    {"[ 进入低功耗睡眠模式 ]"},
    {"[ 控制USB的开关 ]"},
    {"[ 监测电压 电流 功率 ]"},
    {"[ 查看/连接/设置WiFi ]"},
    {"[ 修改设置 ]"},
    {"[ OTA在线升级 ]"},
};

M_SELECT switch_menu[]{
    {"[ USB开关 ]"},
    {"+ USB1"},
    {"+ USB2"},
    {"+ USB3"},
    {"+ USB4"},
};

M_SELECT usb_monitor_menu[]{
    {"USB1"},
    {"USB2"},
    {"USB3"},
    {"USB4"},
    {"总计"},
    {"设置"},
};

M_SELECT usb_monitor_setting_menu[]{
    {"USB监视器设置"},
    {"~ 刷新率Hz"},
    {"+ 串口输出数据"},
};

M_SELECT setting_menu[]{
    // 前缀~代表整数；+代表复选框
    {"[   设置   ]"},
    {"~ 屏幕亮度"},
    {"~ 磁贴动画速度"},
    {"~ 列表动画速度"},
    {"~ 弹窗动画速度"},
    {"~ 聚光动画速度"},
    {"~ 标签动画速度"},
    {"~ 消失动画速度"},
    {"~ 按键短按时长"},
    {"~ 按键长按时长"},
    {"+ 磁贴从头展开"},
    {"+ 列表从头展开"},
    {"+ 磁贴循环"},
    {"+ 列表循环"},
    {"+ 弹窗背景虚化"},
    {"- [ 关于 ]"},
    {"! 重置UI设置"},
};

M_SELECT about_menu[]{
    {"[ ESP32-HUB ]"},
    {"- 作者VRxiaojie"},
    {"- 主控:ESP32S3"},
    {"- SRAM: 8MB"},
    {"- Flash: 8MB"},
    {"- UI修改自WouoUI"},
    {"- 感谢原作者RQNG"},
};

M_SELECT wifi_menu[]{
    {"[ WiFi配置页 ]"},
    {"+ 自动连接WiFi"},
    {"+ 睡眠关闭WiFi"},
    {"- 连接WiFi"},
    {"- 网络信息"},
    {"- WiFi配网"},
    {"- 重置WiFi"},

};

M_SELECT ota_menu[]{
    {"[是否进行OTA升级?]"},
    {" 确定"},
    {" 取消"},
    {""},
};
/************************************* 页面变量 *************************************/

// 修改为默认进入主菜单 原：uint8_t index = M_SLEEP;
// 修改为默认不睡眠。原:bool sleep = true;
ui_t ui = {.index = M_MAIN, .sleep = false, .fade = 1};

tile_t tile = {
    .title_y_calc = TILE_INDI_S + (TILE_INDI_H - TILE_B_TITLE_H) / 2 + TILE_B_TITLE_H * 2,
    .title_y_trg_calc = TILE_INDI_S + (TILE_INDI_H - TILE_B_TITLE_H) / 2 + TILE_B_TITLE_H,
};

list_t list;

usb_monitor_t usb_monitor;

check_box_t check_box;

win_t win;

spot_t spot;

usb_switch_t usb_switch;

/************************************ 初始化函数 ***********************************/

/********************************* 初始化数据处理函数 *******************************/

// 显示数值的初始化
void check_box_v_init(uint8_t *param)
{
    check_box.v = param;
}

// 多选框的初始化
void check_box_m_init(uint8_t *param)
{
    check_box.m = param;
}

// 单选框时的初始化
void check_box_s_init(uint8_t *param, uint8_t *param_p)
{
    check_box.s = param;
    check_box.s_p = param_p;
}

// 多选框处理函数
void check_box_m_select(uint8_t param)
{
    check_box.m[param] = !check_box.m[param];
    eeprom.change = true;
}

// 单选框处理函数
void check_box_s_select(uint8_t val, uint8_t pos)
{
    *check_box.s = val;
    *check_box.s_p = pos;
    eeprom.change = true;
}

// 弹窗数值初始化
void window_value_init(const char *title, uint8_t select, uint8_t *value, uint8_t max, uint8_t min, uint8_t step, MENU *bg, uint8_t index)
{
    win.is_msg = false;
    strcpy(win.title, title);
    win.select = select;
    win.value = value;
    win.max = max;
    win.min = min;
    win.step = step;
    win.bg = bg;
    win.index = index;
    ui.index = M_WINDOW;
    ui.state = S_WINDOW;
}

// 弹窗消息初始化
void window_msg_init(const char *title, const char *sub_title)
{
    win.is_msg = true;
    strcpy(win.title, title);
    strcpy(win.sub_title, sub_title);
    if (ui.index != M_WINDOW)
        win.index = ui.index;
    ui.index = M_WINDOW;
    ui.state = S_WINDOW;
}

/*********************************** UI 初始化函数 *********************************/

// 在初始化EEPROM时，选择性初始化的默认设置
void ui_param_init()
{
    ui.param[DISP_BRI] = 255; // 屏幕亮度
    ui.param[TILE_ANI] = 30;  // 磁贴动画速度
    ui.param[LIST_ANI] = 60;  // 列表动画速度
    ui.param[WIN_ANI] = 25;   // 弹窗动画速度
    ui.param[SPOT_ANI] = 50;  // 聚光动画速度
    ui.param[TAG_ANI] = 60;   // 标签动画速度
    ui.param[FADE_ANI] = 30;  // 消失动画速度
    ui.param[BTN_SPT] = 25;   // 按键短按时长
    ui.param[BTN_LPT] = 150;  // 按键长按时长
    ui.param[TILE_UFD] = 1;   // 磁贴图标从头展开开关
    ui.param[LIST_UFD] = 1;   // 菜单列表从头展开开关
    ui.param[TILE_LOOP] = 0;  // 磁贴图标循环模式开关
    ui.param[LIST_LOOP] = 0;  // 菜单列表循环模式开关
    ui.param[WIN_BOK] = 0;    // 弹窗背景虚化开关
    ui.param[KNOB_DIR] = 0;   // 旋钮方向切换开关
}

// 列表类页面列表行数初始化，必须初始化的参数
void ui_init()
{
    // 修改默认在主菜单的下标为2的磁贴上
    ui.select[0] = 2;
    ui.num[M_MAIN] = sizeof(main_menu) / sizeof(M_SELECT);
    ui.num[M_SWITCH] = sizeof(switch_menu) / sizeof(M_SELECT);
    // ui.num[M_KNOB]      = sizeof( knob_menu     )   / sizeof(M_SELECT);
    // ui.num[M_KRF]       = sizeof( krf_menu      )   / sizeof(M_SELECT);
    // ui.num[M_KPF]       = sizeof( kpf_menu      )   / sizeof(M_SELECT);
    ui.num[M_USB_MONITOR] = sizeof(usb_monitor_menu) / sizeof(M_SELECT);
    ui.num[M_USB_MONITOR_SETTING] = sizeof(usb_monitor_setting_menu) / sizeof(M_SELECT);
    ui.num[M_SETTING] = sizeof(setting_menu) / sizeof(M_SELECT);
    ui.num[M_ABOUT] = sizeof(about_menu) / sizeof(M_SELECT);
    ui.num[M_WIFI] = sizeof(wifi_menu) / sizeof(M_SELECT);
    ui.num[M_OTA] = sizeof(ota_menu) / sizeof(M_SELECT);
}

/********************************* 分页面初始化函数 ********************************/

// 进入磁贴类时的初始化
void tile_param_init()
{
    ui.init = false;
    tile.icon_x = 0;
    tile.icon_x_trg = TILE_ICON_S;
    tile.icon_y = -TILE_ICON_H;
    tile.icon_y_trg = 0;
    tile.indi_x = 0;
    tile.indi_x_trg = TILE_INDI_W;
    tile.title_y = tile.title_y_calc;
    tile.title_y_trg = tile.title_y_trg_calc;
}

// 进入睡眠时的初始化
void sleep_param_init()
{
    u8g2.setDrawColor(0);
    u8g2.drawBox(0, 0, DISP_W, DISP_H);
    u8g2.setPowerSave(1);
    // 睡眠自动关闭WiFi
    if (wifi.param[WIFI_DISABLE_ON_SLEEP])
    {
        WiFi.mode(WIFI_OFF);
    }
    // 设置CPU频率80MHz
    setCpuFrequencyMhz(80);
    ui.state = S_NONE;
    ui.sleep = true;
}

// 开关页初始化
void usb_switch_param_init()
{
    check_box_m_init(usb_switch.switches);
}

// USB监视器测量页初始化
void usb_monitor_param_init()
{
    // 单选框和数值的初始化
    check_box_v_init(usb_monitor.param);
    check_box_m_init(usb_monitor.param);
    usb_monitor.text_bg_l = 0;
    usb_monitor.text_bg_l_trg = DISP_W;
}

// 设置页初始化
void setting_param_init()
{
    check_box_v_init(ui.param);
    check_box_m_init(ui.param);
}

// WIFI页面的初始化
void wifi_init()
{
    check_box_m_init(wifi.param);
    if (WiFi.status() == WL_CONNECTED)
    {
        wifi_menu[3].m_select = "- 断开WiFi";
    }
}

// wifi连接页面显示前的初始化
void wifi_conn_init()
{
    wifi.connectfailed = false;
    // 如果wifi已连接，则直接关闭wifi
    if (WiFi.status() == WL_CONNECTED)
    {
        wifi_menu[3].m_select = "- 连接WiFi";
        WiFi.mode(WIFI_OFF);
        wifi.wifi_power = false;
    }
    // wifi未连接时 尝试连接上次保存的wifi
    else
    {
        wifi.wifi_power = true;
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_wqy12_t_gb2312a);
        u8g2.drawUTF8(0, 16, "尝试连接已保存的wifi");
        u8g2.drawUTF8(0, 32, "请稍候...");
        u8g2.sendBuffer();
        connectToWiFi(3);
    }
}

// wifi配网页面显示前的初始化
void wifi_config_init()
{
    wifi.connectfailed = false;
    initSoftAP();
    initDNS();
    initWebServer();
    // Serial.println("scan start");
    // Serial.println("--------->");
    WiFi.scanNetworks(true); // 异步扫描
}

// OTA页面显示前的初始化
void ota_param_init()
{
    static char t[20];
    sprintf(t, "当前版本:v%s", FIRMWARE_VERSION);
    ota_menu[3].m_select = t;
}

/********************************** 通用初始化函数 *********************************/

/*
  页面层级管理逻辑是，把所有页面都先当作列表类初始化，不是列表类按需求再初始化对应函数
  这样做会浪费一些资源，但跳转页面时只需要考虑页面层级，逻辑上更清晰，减少出错
*/

// 弹窗动画初始化
void window_param_init()
{
    win.bar = 0;
    win.y = WIN_Y;
    win.y_trg = win.u;
    win.y_msg_trg = win.u_msg;
    ui.state = S_NONE;
}

// 进入更深层级时的初始化
void layer_init_in()
{
    ui.layer++;
    ui.init = 0;
    list.y = 0;
    list.y_trg = LIST_LINE_H;
    list.box_x = 0;
    list.box_y = 0;
    list.bar_y = 0;
    ui.state = S_FADE;
    switch (ui.index)
    {
    case M_MAIN:
        tile_param_init();
        break;     // 睡眠进入主菜单，动画初始化
    case M_SWITCH: // 主菜单进入开关页，多选框初始化
        usb_switch_param_init();
        break;
    case M_USB_MONITOR:
        usb_monitor_param_init();
        break; // 主菜单进入电压测量页，动画初始化
    case M_WIFI:
        wifi_init();
        break;
    case M_WIFI_CONN:
        wifi_conn_init();
        break;
    case M_WIFI_CONFIG:
        wifi_config_init();
        break;
    case M_SETTING:
        setting_param_init();
        break; // 主菜单进入设置页，单选框初始化
    case M_OTA:
        ota_param_init();
        break;
    }
}

// 进入更浅层级时的初始化
void layer_init_out()
{
    ui.select[ui.layer] = 0;
    list.box_y_trg[ui.layer] = 0;
    ui.layer--;
    ui.init = 0;
    list.y = 0;
    list.y_trg = LIST_LINE_H;
    list.bar_y = 0;
    ui.state = S_FADE;
    switch (ui.index)
    {
    case M_SLEEP:
        sleep_param_init();
        break; // 主菜单进入睡眠页，检查是否需要写EEPROM
    case M_MAIN:
        tile_param_init();
        break; // 不管什么页面进入主菜单时，动画初始化
    }
}

/************************************* 动画函数 *************************************/

// 动画函数
void animation(float *a, float *a_trg, uint8_t n)
{
    if (*a != *a_trg)
    {
        if (fabs(*a - *a_trg) < 0.15f)
            *a = *a_trg;
        else
            *a += (*a_trg - *a) / (ui.param[n] / 10.0f);
    }
}

// 消失函数
void fade()
{
    delay(ui.param[FADE_ANI]);
    switch (ui.fade)
    {
    case 1:
        for (uint16_t i = 0; i < buf_len; ++i)
            if (i % 2 != 0)
                buf_ptr[i] = buf_ptr[i] & 0xAA;
        break;
    case 2:
        for (uint16_t i = 0; i < buf_len; ++i)
            if (i % 2 != 0)
                buf_ptr[i] = buf_ptr[i] & 0x00;
        break;
    case 3:
        for (uint16_t i = 0; i < buf_len; ++i)
            if (i % 2 == 0)
                buf_ptr[i] = buf_ptr[i] & 0x55;
        break;
    case 4:
        for (uint16_t i = 0; i < buf_len; ++i)
            if (i % 2 == 0)
                buf_ptr[i] = buf_ptr[i] & 0x00;
        break;
    default:
        ui.state = S_NONE;
        ui.fade = 0;
        break;
    }
    ui.fade++;
}

/************************************* 显示函数 *************************************/

// 磁贴类页面通用显示函数
void tile_show(struct MENU arr_1[], struct MENU arr_2[], const uint8_t icon_pic[][16 * 18])
{
    // 计算动画过渡值
    animation(&tile.icon_x, &tile.icon_x_trg, TILE_ANI);
    animation(&tile.icon_y, &tile.icon_y_trg, TILE_ANI);
    animation(&tile.indi_x, &tile.indi_x_trg, TILE_ANI);
    animation(&tile.title_y, &tile.title_y_trg, TILE_ANI);

    // 设置大标题的颜色，0透显，1实显，2反色，这里用实显
    u8g2.setDrawColor(1);
    u8g2.setFontDirection(0);

    // 绘制大标题
    u8g2.setFont(TILE_B_FONT);
    u8g2.drawUTF8(((DISP_W - TILE_INDI_W) - u8g2.getUTF8Width(arr_1[ui.select[ui.layer]].m_select)) / 2 + TILE_INDI_W, tile.title_y, arr_1[ui.select[ui.layer]].m_select);

    // 绘制小标题
    u8g2.setFont(TILE_S_FONT);
    u8g2.drawUTF8(((DISP_W - u8g2.getUTF8Width(arr_2[ui.select[ui.layer]].m_select)) / 2), 0.5 * (TILE_ICON_S + TILE_INDI_H + DISP_H + LIST_TEXT_H), arr_2[ui.select[ui.layer]].m_select);

    // 绘制大标题指示器
    u8g2.drawBox(0, TILE_ICON_S, tile.indi_x, TILE_INDI_H);

    // 绘制图标
    if (!ui.init)
    {
        for (uint8_t i = 0; i < ui.num[ui.index]; ++i)
        {
            if (ui.param[TILE_UFD])
                tile.temp = (DISP_W - TILE_ICON_W) / 2 + i * tile.icon_x - TILE_ICON_S * ui.select[ui.layer];
            else
                tile.temp = (DISP_W - TILE_ICON_W) / 2 + (i - ui.select[ui.layer]) * tile.icon_x;
            u8g2.drawXBMP(tile.temp, (int16_t)tile.icon_y, TILE_ICON_W, TILE_ICON_H, icon_pic[i]);
        }
        if (tile.icon_x == tile.icon_x_trg)
        {
            ui.init = true;
            tile.icon_x = tile.icon_x_trg = -ui.select[ui.layer] * TILE_ICON_S;
        }
    }
    else
        for (uint8_t i = 0; i < ui.num[ui.index]; ++i)
            u8g2.drawXBMP((DISP_W - TILE_ICON_W) / 2 + (int16_t)tile.icon_x + i * TILE_ICON_S, 0, TILE_ICON_W, TILE_ICON_H, icon_pic[i]);
}

/*************** 根据列表每行开头符号，判断每行尾部是否绘制以及绘制什么内容 *************/

// 列表显示数值
void list_draw_value(int n) { u8g2.print(check_box.v[n - 1]); }

// 绘制外框
void list_draw_check_box_frame() { u8g2.drawRFrame(CHECK_BOX_L_S, list.temp + CHECK_BOX_U_S, CHECK_BOX_F_W, CHECK_BOX_F_H, 1); }

// 绘制框里面的点
void list_draw_check_box_dot() { u8g2.drawBox(CHECK_BOX_L_S + CHECK_BOX_D_S + 1, list.temp + CHECK_BOX_U_S + CHECK_BOX_D_S + 1, CHECK_BOX_F_W - (CHECK_BOX_D_S + 1) * 2, CHECK_BOX_F_H - (CHECK_BOX_D_S + 1) * 2); }

// 判断列表尾部内容
void list_draw_text_and_check_box(struct MENU arr[], int i)
{
    u8g2.drawUTF8(LIST_TEXT_S, list.temp + LIST_TEXT_H + LIST_TEXT_S, arr[i].m_select);
    u8g2.setCursor(CHECK_BOX_L_S, list.temp + LIST_TEXT_H + LIST_TEXT_S);
    switch (arr[i].m_select[0])
    {
    case '~':
        list_draw_value(i);
        break;
    case '+':
        list_draw_check_box_frame();
        if (check_box.m[i - 1] == 1)
            list_draw_check_box_dot();
        break;
    case '=':
        list_draw_check_box_frame();
        if (*check_box.s_p == i)
            list_draw_check_box_dot();
        break;
    }
}

/******************************** 列表显示函数 **************************************/

// 列表类页面通用显示函数
void list_show(struct MENU arr[], uint8_t ui_index)
{
    // 更新动画目标值
    u8g2.setFont(LIST_FONT);
    list.box_x_trg = u8g2.getUTF8Width(arr[ui.select[ui.layer]].m_select) + LIST_TEXT_S * 2;
    list.bar_y_trg = ceil((ui.select[ui.layer]) * ((float)DISP_H / (ui.num[ui_index] - 1)));
    // 计算动画过渡值
    animation(&list.y, &list.y_trg, LIST_ANI);
    animation(&list.box_x, &list.box_x_trg, LIST_ANI);
    animation(&list.box_y, &list.box_y_trg[ui.layer], LIST_ANI);
    animation(&list.bar_y, &list.bar_y_trg, LIST_ANI);
    // 检查循环动画是否结束
    if (list.loop && list.box_y == list.box_y_trg[ui.layer])
        list.loop = false;

    // 设置文字和进度条颜色，0透显，1实显，2反色，这里都用实显
    u8g2.setDrawColor(1);
    // 绘制进度条
    u8g2.drawHLine(DISP_W - LIST_BAR_W, 0, LIST_BAR_W);
    u8g2.drawHLine(DISP_W - LIST_BAR_W, DISP_H - 1, LIST_BAR_W);
    u8g2.drawVLine(DISP_W - ceil((float)LIST_BAR_W / 2), 0, DISP_H);
    u8g2.drawBox(DISP_W - LIST_BAR_W, 0, LIST_BAR_W, list.bar_y);
    // 绘制列表文字
    if (!ui.init)
    {
        for (int i = 0; i < ui.num[ui_index]; ++i)
        {
            if (ui.param[LIST_UFD])
                list.temp = i * list.y - LIST_LINE_H * ui.select[ui.layer] + list.box_y_trg[ui.layer];
            else
                list.temp = (i - ui.select[ui.layer]) * list.y + list.box_y_trg[ui.layer];
            list_draw_text_and_check_box(arr, i);
        }
        if (list.y == list.y_trg)
        {
            ui.init = true;
            list.y = list.y_trg = -LIST_LINE_H * ui.select[ui.layer] + list.box_y_trg[ui.layer];
        }
    }
    else
        for (int i = 0; i < ui.num[ui_index]; ++i)
        {
            list.temp = LIST_LINE_H * i + list.y;
            list_draw_text_and_check_box(arr, i);
        }

    // 绘制文字选择框，0透显，1实显，2反色，这里用反色
    u8g2.setDrawColor(2);
    u8g2.drawRBox(0, list.box_y, list.box_x, LIST_LINE_H, LIST_BOX_R);
}

// 电压页面显示函数
void usb_monitor_show()
{
    // 使用列表类显示选项
    u8g2.setFont(LIST_FONT);
    list.box_x_trg = u8g2.getUTF8Width(usb_monitor_menu[ui.select[ui.layer]].m_select) + LIST_TEXT_S * 2;

    // 计算动画过渡值
    animation(&list.y, &list.y_trg, LIST_ANI);
    animation(&list.box_x, &list.box_x_trg, LIST_ANI);
    animation(&list.box_y, &list.box_y_trg[ui.layer], LIST_ANI);
    animation(&usb_monitor.text_bg_l, &usb_monitor.text_bg_l_trg, TAG_ANI);

    // 检查循环动画是否结束
    if (list.loop && list.box_y == list.box_y_trg[ui.layer])
        list.loop = false;

    // 设置曲线颜色，0透显，1实显，2反色，这里用实显
    u8g2.setDrawColor(1);

    // 绘制列表文字
    u8g2.setFontDirection(1);
    if (!ui.init)
    {
        for (uint8_t i = 0; i < ui.num[ui.index]; ++i)
            u8g2.drawUTF8(LIST_TEXT_S + (i - ui.select[ui.layer]) * list.y + list.box_y_trg[ui.layer] - 1, USB_MONITOR_LIST_U_S, usb_monitor_menu[i].m_select);
        if (list.y == list.y_trg)
        {
            ui.init = true;
            list.y = list.y_trg = -LIST_LINE_H * ui.select[ui.layer] + list.box_y_trg[ui.layer];
        }
    }
    else
        for (uint8_t i = 0; i < ui.num[ui.index]; ++i)
            u8g2.drawUTF8(LIST_TEXT_S + LIST_LINE_H * i + (int16_t)list.y - 1, USB_MONITOR_LIST_U_S, usb_monitor_menu[i].m_select);

    // 根据当前选择的USB端口，绘制当前USB的电压、电流、功率
    uint32_t idx = ui.select[ui.layer];
    u8g2.setFontDirection(0);
    float maxBusVolt = 0;
    float totalCurrent_mA = 0;
    float totalPower_mW = 0;
    switch (idx)
    {
    // 根据当前选择的USB端口(1~4)，通知INA226任务去获取对应通道的数据
    case 0:
    case 1:
    case 2:
    case 3:
        if (ina226_data[idx].init)
        {

            u8g2.setFont(USB_MONITOR_FONT);
            u8g2.setCursor(0, 28);
            u8g2.printf("%1.2f", ina226_data[idx].busVoltage);
            u8g2.setFont(USB_MONITOR_UNIT_FONT);
            u8g2.setCursor(101, 23);
            u8g2.print("V");

            u8g2.setFont(USB_MONITOR_FONT);
            u8g2.setCursor(0, 56);
            u8g2.printf("%3.1f", ina226_data[idx].current_mA);
            u8g2.setFont(USB_MONITOR_UNIT_FONT);
            u8g2.setCursor(92, 51);
            u8g2.print("mA");

            u8g2.setFont(USB_MONITOR_FONT);
            u8g2.setCursor(0, 84);
            if (ina226_data[idx].power_mW < 1000)
            {
                u8g2.printf("%3.1f", ina226_data[idx].power_mW);
                u8g2.setFont(USB_MONITOR_UNIT_FONT);
                u8g2.setCursor(92, 79);
                u8g2.print("mW");
            }
            else
            {
                u8g2.printf("%2.2f", ina226_data[idx].power_mW / 1000.0);
                u8g2.setFont(USB_MONITOR_UNIT_FONT);
                u8g2.setCursor(101, 79);
                u8g2.print("W");
            }
        }
        else
        {
            u8g2.setFont(USB_MONITOR_FONT);
            u8g2.setCursor(0, 30);
            u8g2.print("NO");
            u8g2.setCursor(0, 60);
            u8g2.print("DATA");
        }
        break;
    case 4: // 总计功能
        for (uint8_t i = 0; i < 4; i++)
        {
            if (ina226_data[i].init)
            {
                if (ina226_data[i].busVoltage > maxBusVolt)
                    maxBusVolt = ina226_data[i].busVoltage;
                totalCurrent_mA += ina226_data[i].current_mA;
                totalPower_mW += ina226_data[i].busVoltage * ina226_data[i].current_mA;
            }
        }

        u8g2.setFont(USB_MONITOR_FONT);
        u8g2.setCursor(0, 28);
        u8g2.printf("%1.2f", maxBusVolt);
        u8g2.setFont(USB_MONITOR_UNIT_FONT);
        u8g2.setCursor(101, 23);
        u8g2.print("V");

        u8g2.setFont(USB_MONITOR_FONT);
        u8g2.setCursor(0, 56);
        if (totalCurrent_mA < 1000)
        {
            u8g2.printf("%3.1f", totalCurrent_mA);
            u8g2.setFont(USB_MONITOR_UNIT_FONT);
            u8g2.setCursor(92, 51);
            u8g2.print("mA");
        }
        else
        {
            u8g2.printf("%1.3f", totalCurrent_mA / 1000.0);
            u8g2.setFont(USB_MONITOR_UNIT_FONT);
            u8g2.setCursor(101, 51);
            u8g2.print("A");
        }

        u8g2.setFont(USB_MONITOR_FONT);
        u8g2.setCursor(0, 84);
        if (totalPower_mW < 1000)
        {
            u8g2.printf("%3.1f", totalPower_mW);
            u8g2.setFont(USB_MONITOR_UNIT_FONT);
            u8g2.setCursor(92, 79);
            u8g2.print("mW");
        }
        else
        {
            u8g2.printf("%2.2f", totalPower_mW / 1000.0);
            u8g2.setFont(USB_MONITOR_UNIT_FONT);
            u8g2.setCursor(101, 79);
            u8g2.print("W");
        }
        break;
    case 5: // 进入USB监视器的设置页面
        u8g2.setFont(USB_MONITOR_UNIT_FONT);
        u8g2.setCursor(0, 20);
        u8g2.print("Press Btn");
        u8g2.setCursor(0, 40);
        u8g2.print("to Modify");
        u8g2.setCursor(0, 60);
        u8g2.print("Settings");
        break;
    }

    // 绘制列表选择框
    u8g2.setDrawColor(2);
    u8g2.drawRBox(list.box_y, USB_MONITOR_LIST_U_S - LIST_TEXT_S, LIST_LINE_H, list.box_x, LIST_BOX_R); // 列表选择框
}

// 弹窗通用显示函数
void window_show()
{
    // 绘制背景列表，根据开关判断背景是否要虚化
    list_show(win.bg, win.index);
    if (ui.param[WIN_BOK])
        for (uint16_t i = 0; i < buf_len; ++i)
            buf_ptr[i] = buf_ptr[i] & (i % 2 == 0 ? 0x55 : 0xAA);

    // 更新动画目标值
    u8g2.setFont(WIN_FONT);
    win.bar_trg = (float)(*win.value - win.min) / (float)(win.max - win.min) * (WIN_BAR_W - 4);

    // 计算动画过渡值
    animation(&win.bar, &win.bar_trg, WIN_ANI);
    animation(&win.y, &win.y_trg, WIN_ANI);

    // 绘制窗口
    u8g2.setDrawColor(0);
    u8g2.drawRBox(win.l, (int16_t)win.y, WIN_W, WIN_H, 2); // 绘制外框背景
    u8g2.setDrawColor(1);
    u8g2.drawRFrame(win.l, (int16_t)win.y, WIN_W, WIN_H, 2);                  // 绘制外框描边
    u8g2.drawRFrame(win.l + 5, (int16_t)win.y + 20, WIN_BAR_W, WIN_BAR_H, 1); // 绘制进度条外框
    u8g2.drawBox(win.l + 7, (int16_t)win.y + 22, win.bar, WIN_BAR_H - 4);     // 绘制进度条
    u8g2.setCursor(win.l + 5, (int16_t)win.y + 14);
    u8g2.print(win.title); // 绘制标题
    u8g2.setCursor(win.l + 78, (int16_t)win.y + 14);
    u8g2.print(*win.value); // 绘制当前值

    // 需要在窗口修改参数时立即见效的函数
    if (!strcmp(win.title, "Disp Bri"))
        u8g2.setContrast(ui.param[DISP_BRI]);
}

/************************************* 处理函数 *************************************/

/*********************************** 通用处理函数 ***********************************/

// 磁贴类页面旋转时判断通用函数
void tile_rotate_switch()
{
    switch (btn.id)
    {
    case BTN_ID_CC:
        if (ui.init)
        {
            if (ui.select[ui.layer] > 0)
            {
                ui.select[ui.layer] -= 1;
                tile.icon_x_trg += TILE_ICON_S;
                tile.select_flag = false;
            }
            else
            {
                if (ui.param[TILE_LOOP])
                {
                    ui.select[ui.layer] = ui.num[ui.index] - 1;
                    tile.icon_x_trg = -TILE_ICON_S * (ui.num[ui.index] - 1);
                    break;
                }
                else
                    tile.select_flag = true;
            }
        }
        break;

    case BTN_ID_CW:
        if (ui.init)
        {
            if (ui.select[ui.layer] < (ui.num[ui.index] - 1))
            {
                ui.select[ui.layer] += 1;
                tile.icon_x_trg -= TILE_ICON_S;
                tile.select_flag = false;
            }
            else
            {
                if (ui.param[TILE_LOOP])
                {
                    ui.select[ui.layer] = 0;
                    tile.icon_x_trg = 0;
                    break;
                }
                else
                    tile.select_flag = true;
            }
        }
        break;
    }
}

// 列表类页面旋转时判断通用函数
void list_rotate_switch()
{
    if (!list.loop)
    {
        switch (btn.id)
        {
        case BTN_ID_CC:
            if (ui.select[ui.layer] == 0)
            {
                if (ui.param[LIST_LOOP] && ui.init)
                {
                    list.loop = true;
                    ui.select[ui.layer] = ui.num[ui.index] - 1;
                    if (ui.num[ui.index] > list.line_n)
                    {
                        list.box_y_trg[ui.layer] = DISP_H - LIST_LINE_H;
                        list.y_trg = DISP_H - ui.num[ui.index] * LIST_LINE_H;
                    }
                    else
                        list.box_y_trg[ui.layer] = (ui.num[ui.index] - 1) * LIST_LINE_H;
                    break;
                }
                else
                    break;
            }
            if (ui.init)
            {
                ui.select[ui.layer] -= 1;
                if (ui.select[ui.layer] < -(list.y_trg / LIST_LINE_H))
                {
                    if (!(DISP_H % LIST_LINE_H))
                        list.y_trg += LIST_LINE_H;
                    else
                    {
                        if (list.box_y_trg[ui.layer] == DISP_H - LIST_LINE_H * list.line_n)
                        {
                            list.y_trg += (list.line_n + 1) * LIST_LINE_H - DISP_H;
                            list.box_y_trg[ui.layer] = 0;
                        }
                        else if (list.box_y_trg[ui.layer] == LIST_LINE_H)
                        {
                            list.box_y_trg[ui.layer] = 0;
                        }
                        else
                            list.y_trg += LIST_LINE_H;
                    }
                }
                else
                    list.box_y_trg[ui.layer] -= LIST_LINE_H;
                break;
            }

        case BTN_ID_CW:
            if (ui.select[ui.layer] == (ui.num[ui.index] - 1))
            {
                if (ui.param[LIST_LOOP] && ui.init)
                {
                    list.loop = true;
                    ui.select[ui.layer] = 0;
                    list.y_trg = 0;
                    list.box_y_trg[ui.layer] = 0;
                    break;
                }
                else
                    break;
            }
            if (ui.init)
            {
                ui.select[ui.layer] += 1;
                if ((ui.select[ui.layer] + 1) > (list.line_n - list.y_trg / LIST_LINE_H))
                {
                    if (!(DISP_H % LIST_LINE_H))
                        list.y_trg -= LIST_LINE_H;
                    else
                    {
                        if (list.box_y_trg[ui.layer] == LIST_LINE_H * (list.line_n - 1))
                        {
                            list.y_trg -= (list.line_n + 1) * LIST_LINE_H - DISP_H;
                            list.box_y_trg[ui.layer] = DISP_H - LIST_LINE_H;
                        }
                        else if (list.box_y_trg[ui.layer] == DISP_H - LIST_LINE_H * 2)
                        {
                            list.box_y_trg[ui.layer] = DISP_H - LIST_LINE_H;
                        }
                        else
                            list.y_trg -= LIST_LINE_H;
                    }
                }
                else
                    list.box_y_trg[ui.layer] += LIST_LINE_H;
                break;
            }
            break;
        }
    }
}

// 弹窗通用处理函数
void window_proc()
{
    if (win.is_msg)
    {
        // 更新动画目标值
        u8g2.setFont(WIN_MSG_FONT);
        // 计算动画过渡值
        animation(&win.y, &win.y_msg_trg, WIN_ANI);
        // 绘制窗口
        u8g2.setDrawColor(0);
        u8g2.drawRBox(win.l_msg, (int16_t)win.y, WIN_MSG_W, WIN_MSG_H, 2); // 绘制外框背景
        u8g2.setDrawColor(1);
        u8g2.drawRFrame(win.l_msg, (int16_t)win.y, WIN_MSG_W, WIN_MSG_H, 2); // 绘制外框描边
        // 绘制标题
        u8g2.drawUTF8(win.l_msg + 5, (int16_t)win.y + 14, win.title);
        // 绘制小标题
        u8g2.drawUTF8(win.l_msg + 5, (int16_t)win.y + 26, win.sub_title);
        // 绘制OK按钮
        u8g2.setCursor(WIN_MSG_W / 2 - 9, (int16_t)win.y + 42);
        u8g2.print("OK");
        u8g2.setDrawColor(2);
        u8g2.drawRBox(WIN_MSG_W / 2 - 9 - 4, (int16_t)win.y + 30, 2 * 9 + 6, 14, 0.5f);

        if (btn.pressed && win.y == win.y_msg_trg && win.y != WIN_Y_TRG)
        {
            btn.pressed = false;
            // 返回上一级
            switch (btn.id)
            {
            case BTN_ID_SP:
            case BTN_ID_LP:
                ui.index = win.index;
                // 对于在主菜单的弹窗，手动播放磁贴动画
                if (ui.index == M_MAIN)
                    tile_param_init();
                break;
            }
        }
    }
    else
    {
        window_show();
        if (win.y == WIN_Y_TRG)
            ui.index = win.index;
        if (btn.pressed && win.y == win.y_trg && win.y != WIN_Y_TRG)
        {
            btn.pressed = false;
            switch (btn.id)
            {
            case BTN_ID_CW:
                if (*win.value < win.max)
                    *win.value += win.step;
                eeprom.change = true;
                break;
            case BTN_ID_CC:
                if (*win.value > win.min)
                    *win.value -= win.step;
                eeprom.change = true;
                break;
            case BTN_ID_SP:
            case BTN_ID_LP:
                win.y_trg = WIN_Y_TRG;
                break;
            }
        }
    }
}

/********************************** 分页面处理函数 **********************************/

// 睡眠页面处理函数
void sleep_proc()
{
    while (ui.sleep)
    {
        // 睡眠时循环执行的函数

        // 睡眠时需要扫描旋钮才能退出睡眠
        // btn_scan();
        // 当旋钮有动作时
        if (btn.pressed)
        {
            btn.pressed = false;
            ui.index = M_MAIN;
            ui.state = S_LAYER_IN;
            u8g2.setPowerSave(0);
            ui.sleep = false;
            // 从睡眠恢复自动连接WiFi
            if (wifi.param[WIFI_DISABLE_ON_SLEEP])
            {
                WiFi.begin();
            }
            // 恢复CPU频率为240MHz
            setCpuFrequencyMhz(240);
        }
        delay(100);
    }
}

// 主菜单处理函数，磁贴类模板
void main_proc()
{
    tile_show(main_menu, main_menu_exp, main_icon_pic);
    if (btn.pressed)
    {
        btn.pressed = false;
        switch (btn.id)
        {
        case BTN_ID_CW:
        case BTN_ID_CC:
            tile_rotate_switch();
            break;
        case BTN_ID_SP:
            switch (ui.select[ui.layer])
            {
            case 0:
                ui.index = M_SLEEP;
                ui.state = S_LAYER_OUT;
                break;
            case 1:
                ui.index = M_SWITCH;
                ui.state = S_LAYER_IN;
                break;
            case 2:
                ui.index = M_USB_MONITOR;
                ui.state = S_LAYER_IN;
                break;
            case 3:
                ui.index = M_WIFI;
                ui.state = S_LAYER_IN;
                break;
            case 4:
                ui.index = M_SETTING;
                ui.state = S_LAYER_IN;
                break;
            case 5:
                ui.index = M_OTA;
                ui.state = S_LAYER_IN;
                break;
            }
        }
        if (!tile.select_flag && ui.init)
        {
            tile.indi_x = 0;
            tile.title_y = tile.title_y_calc;
        }
    }
}

// 开关菜单处理函数
void switch_proc()
{
    list_show(switch_menu, M_SWITCH);
    if (btn.pressed)
    {
        btn.pressed = false;
        switch (btn.id)
        {
        case BTN_ID_CW:
        case BTN_ID_CC:
            list_rotate_switch();
            break;
        case BTN_ID_LP:
            ui.select[ui.layer] = 0;
        case BTN_ID_SP:
            switch (ui.select[ui.layer])
            {

            case 0:
                ui.index = M_MAIN;
                ui.state = S_LAYER_OUT;
                break;
            case 1:
                check_box_m_select(SW1);
                switch_ctrl(SW1, check_box.m[SW1]);
                break;
            case 2:
                check_box_m_select(SW2);
                switch_ctrl(SW2, check_box.m[SW2]);
                break;
            case 3:
                check_box_m_select(SW3);
                switch_ctrl(SW3, check_box.m[SW3]);
                break;
            case 4:
                check_box_m_select(SW4);
                switch_ctrl(SW4, check_box.m[SW4]);
                break;
                // case 11:  ui.index = M_KNOB;  ui.state = S_LAYER_IN;  break;
            }
        }
    }
}

// 电压测量设置页处理函数
void usb_monitor_setting_proc()
{
    list_show(usb_monitor_setting_menu, M_USB_MONITOR_SETTING);
    if (btn.pressed)
    {
        btn.pressed = false;
        switch (btn.id)
        {
        case BTN_ID_CW:
        case BTN_ID_CC:
            list_rotate_switch();
            break;
        case BTN_ID_LP:
            ui.select[ui.layer] = 0;
            // 退出菜单时保存
            if (eeprom.change == true)
            {
                eeprom.change = false;
                EEPROM_write_monitor_setting();
            }
        case BTN_ID_SP:
            switch (ui.select[ui.layer])
            {
            case 0:
                ui.index = M_USB_MONITOR;
                ui.state = S_LAYER_OUT;
                break;
            case 1: // 调整刷新间隔
                window_value_init("Refresh Rate", REFRESH_RATE, &usb_monitor.param[REFRESH_RATE],
                                  10, 1, 1, usb_monitor_setting_menu, M_USB_MONITOR_SETTING);
                break;
            case 2: // 串口输出电压电流
                check_box_m_select(MONITOR_SERIAL_OUTPUT);
                break;
            }
        }
    }
}

// 电压测量页处理函数
void usb_monitor_proc()
{
    usb_monitor_show();
    if (btn.pressed)
    {
        btn.pressed = false;
        switch (btn.id)
        {
        case BTN_ID_CW:
        case BTN_ID_CC:
            list_rotate_switch();
            break;

        case BTN_ID_SP:
            switch (ui.select[ui.layer])
            {
            case 5: // USB监视器的设置
                ui.index = M_USB_MONITOR_SETTING;
                ui.state = S_LAYER_IN;
                break;
            }

            break;
        case BTN_ID_LP:
            ui.index = M_MAIN;
            ui.state = S_LAYER_OUT;
            break;
        }
    }
}

// 设置菜单处理函数，多选框列表类模板，弹窗模板
void setting_proc()
{
    list_show(setting_menu, M_SETTING);
    if (btn.pressed)
    {
        btn.pressed = false;
        switch (btn.id)
        {
        case BTN_ID_CW:
        case BTN_ID_CC:
            list_rotate_switch();
            break;
        case BTN_ID_LP:
            ui.select[ui.layer] = 0;
            // 退出菜单时保存
            if (eeprom.change == true)
            {
                eeprom.change = false;
                EEPROM_write_ui_setting(true);
            }
        case BTN_ID_SP:
            switch (ui.select[ui.layer])
            {

            // 返回更浅层级，长按被当作选择这一项，也是执行这一行
            case 0:
                ui.index = M_MAIN;
                ui.state = S_LAYER_OUT;
                break;

            // 弹出窗口，参数初始化：标题，参数名，参数值，最大值，最小值，步长，背景列表名，背景列表标签
            case 1:
                window_value_init("Disp Bri", DISP_BRI, &ui.param[DISP_BRI], 255, 0, 5, setting_menu, M_SETTING);
                break;
            case 2:
                window_value_init("Tile Ani", TILE_ANI, &ui.param[TILE_ANI], 100, 10, 1, setting_menu, M_SETTING);
                break;
            case 3:
                window_value_init("List Ani", LIST_ANI, &ui.param[LIST_ANI], 100, 10, 1, setting_menu, M_SETTING);
                break;
            case 4:
                window_value_init("Win Ani", WIN_ANI, &ui.param[WIN_ANI], 100, 10, 1, setting_menu, M_SETTING);
                break;
            case 5:
                window_value_init("Spot Ani", SPOT_ANI, &ui.param[SPOT_ANI], 100, 10, 1, setting_menu, M_SETTING);
                break;
            case 6:
                window_value_init("Tag Ani", TAG_ANI, &ui.param[TAG_ANI], 100, 10, 1, setting_menu, M_SETTING);
                break;
            case 7:
                window_value_init("Fade Ani", FADE_ANI, &ui.param[FADE_ANI], 255, 0, 1, setting_menu, M_SETTING);
                break;
            case 8:
                window_value_init("Btn SPT", BTN_SPT, &ui.param[BTN_SPT], 255, 0, 1, setting_menu, M_SETTING);
                break;
            case 9:
                window_value_init("Btn LPT", BTN_LPT, &ui.param[BTN_LPT], 255, 0, 1, setting_menu, M_SETTING);
                break;

            // 多选框
            case 10:
                check_box_m_select(TILE_UFD);
                break;
            case 11:
                check_box_m_select(LIST_UFD);
                break;
            case 12:
                check_box_m_select(TILE_LOOP);
                break;
            case 13:
                check_box_m_select(LIST_LOOP);
                break;
            case 14:
                check_box_m_select(WIN_BOK);
                break;

            // 关于本机
            case 15:
                ui.index = M_ABOUT;
                ui.state = S_LAYER_IN;
                break;
                // 重置UI设置
            case 16:
                ui_param_init();
                EEPROM_write_ui_setting(false); // 初始化所有UI设置
                window_msg_init("UI设置", "重置成功!");
                break;
            }
        }
    }
}

// 关于本机页
void about_proc()
{
    list_show(about_menu, M_ABOUT);
    if (btn.pressed)
    {
        btn.pressed = false;
        switch (btn.id)
        {
        case BTN_ID_CW:
        case BTN_ID_CC:
            list_rotate_switch();
            break;
        case BTN_ID_LP:
            ui.select[ui.layer] = 0;
        case BTN_ID_SP:
            switch (ui.select[ui.layer])
            {

            case 0:
                ui.index = M_SETTING;
                ui.state = S_LAYER_OUT;
                break;
            }
        }
    }
}

void wifi_conn_proc()
{
    char buffer[20];
    if (btn.pressed)
    {
        btn.pressed = false;
        switch (btn.id)
        {
        case BTN_ID_LP:
            ui.select[ui.layer] = 0;
        case BTN_ID_SP:
            ui.index = M_WIFI;
            ui.state = S_LAYER_OUT;
            break;
        }
    }

    if (wifi.wifi_power == true)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            wifi_menu[3].m_select = "- 断开WiFi";
            u8g2.clearBuffer();
            u8g2.setDrawColor(1);
            u8g2.setFont(u8g2_font_wqy12_t_gb2312a);
            u8g2.drawUTF8(0, 16, "连接成功!");
            u8g2.drawUTF8(0, 32, WiFi.SSID().c_str()); // WiFi SSID
            snprintf(buffer, sizeof(buffer), "ip: %s", WiFi.localIP().toString().c_str());
            u8g2.drawUTF8(0, 48, buffer);
            snprintf(buffer, sizeof(buffer), "DNS: %s", WiFi.dnsIP().toString().c_str());
            u8g2.drawUTF8(0, 64, buffer);
            u8g2.drawUTF8(0, 80, "长按返回主菜单");
            u8g2.sendBuffer();
        }
        else
        {
            u8g2.clearBuffer();
            u8g2.setDrawColor(1);
            u8g2.setFont(u8g2_font_wqy12_t_gb2312a);
            u8g2.drawUTF8(0, 16, "连接失败!");
            u8g2.drawUTF8(0, 32, "请重试或重新配网");
            u8g2.drawUTF8(0, 48, "长按返回");
            u8g2.sendBuffer();
        }
    }
    else
    {
        u8g2.setDrawColor(1);
        u8g2.setFont(u8g2_font_wqy12_t_gb2312a);
        u8g2.drawUTF8(0, 16, "WiFi已断开");
        u8g2.drawUTF8(0, 32, "1s后自动返回");
        u8g2.sendBuffer();
        delay(1000);
        ui.index = M_WIFI;
        ui.state = S_LAYER_OUT;
    }
}

void wifi_info_proc()
{
    char buffer[20];
    if (btn.pressed)
    {
        btn.pressed = false;
        switch (btn.id)
        {
        case BTN_ID_LP:
            ui.select[ui.layer] = 0;
        case BTN_ID_SP:
            ui.index = M_WIFI;
            ui.state = S_LAYER_OUT;
            break;
        }
    }
    u8g2.setDrawColor(1);
    u8g2.setFont(u8g2_font_wqy12_t_gb2312a);
    u8g2.drawUTF8(0, 16, "[ 网络信息 ]");
    if (WiFi.status() == WL_CONNECTED)
        u8g2.drawUTF8(0, 32, WiFi.SSID().c_str()); // WiFi SSID
    else
        u8g2.drawUTF8(0, 32, "WiFi未连接!");
    snprintf(buffer, sizeof(buffer), "ip: %s", WiFi.localIP().toString().c_str());
    u8g2.drawUTF8(0, 48, buffer);
    snprintf(buffer, sizeof(buffer), "DNS: %s", WiFi.dnsIP().toString().c_str());
    u8g2.drawUTF8(0, 64, buffer);
    u8g2.drawUTF8(0, 80, "长按返回");
    u8g2.sendBuffer();
}

void wifi_config_proc()
{
    char buffer[20];
    if (btn.pressed)
    {
        btn.pressed = false;
        switch (btn.id)
        {
        case BTN_ID_LP:
            ui.select[ui.layer] = 0;
        case BTN_ID_SP:
            ui.index = M_WIFI;
            ui.state = S_LAYER_OUT;
            WiFi.scanDelete();
            WiFi.mode(WIFI_OFF); // 关闭WiFi
            break;
        }
    }

    int16_t WiFi_scan_status = WiFi.scanComplete();
    if (WiFi_scan_status >= 0) // WiFi扫描完成 WiFi_status为获取到的网络数量
    {
        scanWiFi(WiFi_scan_status);
        while (1)
        {
            uint8_t WiFi_status = WiFi.status();
            checkDNS_HTTP(); // 检测客户端DNS&HTTP请求，也就是检查配网页面那部分
            if (btn.pressed)
            {
                btn.pressed = false;
                switch (btn.id)
                {
                case BTN_ID_LP:
                    ui.select[ui.layer] = 0;
                case BTN_ID_SP:
                    switch (ui.select[ui.layer])
                    {
                    case 0:
                        ui.index = M_WIFI;
                        ui.state = S_LAYER_OUT;
                        WiFi.scanDelete();
                        return;
                    }
                    break;
                }
            }
            if (WiFi_status == WL_CONNECTED) // 检测到连接成功后 显示网络信息
            {
                wifi_menu[3].m_select = "- 断开WiFi";
                u8g2.clearBuffer();
                u8g2.setDrawColor(1);
                u8g2.setFont(u8g2_font_wqy12_t_gb2312a);
                u8g2.drawUTF8(0, 16, "连接成功!");
                u8g2.drawUTF8(0, 32, WiFi.SSID().c_str()); // WiFi SSID
                snprintf(buffer, sizeof(buffer), "ip: %s", WiFi.localIP().toString().c_str());
                u8g2.drawUTF8(0, 48, buffer);
                snprintf(buffer, sizeof(buffer), "DNS: %s", WiFi.dnsIP().toString().c_str());
                u8g2.drawUTF8(0, 64, buffer);
                u8g2.drawUTF8(0, 80, "长按返回");
                u8g2.sendBuffer();
            }
            else if (wifi.connectfailed == true) // 连接失败时显示提示信息
            {
                u8g2.clearBuffer();
                u8g2.drawUTF8(0, 16, "WiFi连接失败!");
                u8g2.drawUTF8(0, 32, "请稍后重新进入该页面!");
                u8g2.sendBuffer();
                delay(1500);
                ui.index = M_WIFI; // 退出配网页面
                ui.state = S_LAYER_OUT;
                WiFi.scanDelete();
                WiFi.mode(WIFI_OFF); // 关闭WiFi
                wifi.connectfailed = false;
                return;
            }
            else // 若还未连接 则显示提示信息
            {
                u8g2.drawUTF8(0, 16, "[WiFi配网]长按退出");
                u8g2.drawUTF8(0, 32, "手机连到热点:Hub-WiFi");
                u8g2.drawUTF8(0, 48, "进入网页:192.168.4.1");
                u8g2.drawUTF8(0, 64, "根据提示操作");
                u8g2.sendBuffer();
            }
        }
    }
    if (WiFi_scan_status == WIFI_SCAN_RUNNING) // WiFi状态为扫描中
    {
        u8g2.setDrawColor(1);
        u8g2.setFont(u8g2_font_wqy12_t_gb2312a);
        u8g2.drawUTF8(0, 16, "[配网]");
        u8g2.drawUTF8(0, 32, "扫描WIFI中,请稍候...");
        u8g2.drawUTF8(0, 48, "长按退出");
        u8g2.sendBuffer();
    }
}

void wifi_proc()
{
    list_show(wifi_menu, M_WIFI);
    if (btn.pressed)
    {
        btn.pressed = false;
        switch (btn.id)
        {
        case BTN_ID_CW:
        case BTN_ID_CC:
            list_rotate_switch();
            break;
        case BTN_ID_LP:
            if (eeprom.change == true)
            {
                eeprom.change = false;
                EEPROM_write_wifi_setting();
            }
            ui.select[ui.layer] = 0;
        case BTN_ID_SP:
            switch (ui.select[ui.layer])
            {
            case 0:
                ui.index = M_MAIN;
                ui.state = S_LAYER_OUT;
                WiFi.scanDelete();
                break;
            case 1: // 开机自动连接WIFI选项
                check_box_m_select(WIFI_AUTO_CONN);
                break;
            case 2: // 睡眠时关闭WIFI选项
                check_box_m_select(WIFI_DISABLE_ON_SLEEP);
                break;
            case 3: // 连接WiFi
                ui.index = M_WIFI_CONN;
                ui.state = S_LAYER_IN;
                break;
            case 4: // 网络信息
                ui.index = M_WIFI_INFO;
                ui.state = S_LAYER_IN;
                break;
            case 5: // 配网
                ui.index = M_WIFI_CONFIG;
                ui.state = S_LAYER_IN;
                break;
            case 6: // 重置WiFi
                if (restoreWiFi())
                {
                    wifi_menu[3].m_select = "- 断开WiFi";
                    window_msg_init("WiFi重置成功", "");
                }
                else
                    window_msg_init("WiFi重置失败", "请重试");
                break;
            }
            break;
        }
    }
}

void ota_proc()
{
    list_show(ota_menu, M_OTA);
    if (btn.pressed)
    {
        btn.pressed = false;
        switch (btn.id)
        {
        case BTN_ID_CW:
        case BTN_ID_CC:
            list_rotate_switch();
            break;
        case BTN_ID_LP:
            ui.select[ui.layer] = 0;
        case BTN_ID_SP:
            switch (ui.select[ui.layer])
            {
            case 0:
                ui.index = M_MAIN;
                ui.state = S_LAYER_OUT;
                break;
            case 1: // 确定
                if (WL_CONNECTED == WiFi.status())
                {
                    // 如果有新版本，则写入OTA标志位并重启
                    if (checkForOTA())
                    {
                        EEPROM_write_ota_flag(true);
                        ESP.restart();
                    }
                    else
                    {
                        ui.index = M_MAIN;
                        ui.state = S_LAYER_OUT;
                    }
                }
                else
                {
                    window_msg_init("WiFi未连接!", "请检查网络连接");
                }
                break;
            case 2: // 取消
                ui.index = M_MAIN;
                ui.state = S_LAYER_OUT;
                break;
            }
        }
    }
}

void ui_proc()
{
    u8g2.sendBuffer();
    switch (ui.state)
    {
    case S_FADE:
        fade();
        break; // 转场动画
    case S_WINDOW:
        window_param_init();
        break; // 弹窗初始化
    case S_LAYER_IN:
        layer_init_in();
        break; // 层级初始化
    case S_LAYER_OUT:
        layer_init_out();
        break; // 层级初始化

    case S_NONE:
        u8g2.clearBuffer();
        switch (ui.index) // 直接选择页面
        {
        case M_WINDOW:
            window_proc();
            break;
        case M_SLEEP:
            sleep_proc();
            break;
        case M_MAIN:
            main_proc();
            break;
        case M_SWITCH:
            switch_proc();
            break;
        // case M_KNOB:        knob_proc();              break;
        // case M_KRF:         krf_proc();               break;
        // case M_KPF:         kpf_proc();               break;
        case M_USB_MONITOR:
            usb_monitor_proc();
            break;
        case M_USB_MONITOR_SETTING:
            usb_monitor_setting_proc();
            break;
        case M_WIFI:
            wifi_proc();
            break;
        case M_WIFI_CONN:
            wifi_conn_proc();
            break;
        case M_WIFI_INFO:
            wifi_info_proc();
            break;
        case M_WIFI_CONFIG:
            wifi_config_proc();
            break;
        case M_SETTING:
            setting_proc();
            break;
        case M_ABOUT:
            about_proc();
            break;
        case M_OTA:
            ota_proc();
            break;
        }
    }
}
