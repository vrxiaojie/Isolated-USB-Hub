#ifndef UI_H_
#define UI_H_
#include <Arduino.h>
#include "oled.h"
#include "OTA.h"

// UI变量
#define UI_DEPTH 20  // 最深层级数
#define UI_MNUMB 100 // 菜单数量
#define UI_PARAM 14  // 参数数量
enum
{
    DISP_BRI,  // 屏幕亮度
    TILE_ANI,  // 磁贴动画速度
    LIST_ANI,  // 列表动画速度
    WIN_ANI,   // 弹窗动画速度
    SPOT_ANI,  // 聚光动画速度
    TAG_ANI,   // 标签动画速度
    FADE_ANI,  // 消失动画速度
    BTN_SPT,   // 按键短按时长
    BTN_LPT,   // 按键长按时长
    TILE_UFD,  // 磁贴图标从头展开开关
    LIST_UFD,  // 菜单列表从头展开开关
    TILE_LOOP, // 磁贴图标循环模式开关
    LIST_LOOP, // 菜单列表循环模式开关
    WIN_BOK,   // 弹窗背景虚化开关
    KNOB_DIR,  // 旋钮方向切换开关
    DARK_MODE, // 黑暗模式开关
};

struct ui_t
{
    bool init;
    uint8_t num[UI_MNUMB];
    uint8_t select[UI_DEPTH];
    uint8_t layer;
    uint8_t index;
    uint8_t state;
    bool sleep;
    uint8_t fade;
    uint8_t param[UI_PARAM];
};
extern ui_t ui;

/************************************* 定义页面 *************************************/

// 总目录，缩进表示页面层级
enum
{
    M_WINDOW,
    M_SLEEP,
    M_MAIN,
    M_SWITCH,
    M_USB_MONITOR,
    M_USB_MONITOR_SETTING,
    M_WIFI,
    M_WIFI_CONN,   // 连接wifi页
    M_WIFI_INFO,   // 网络信息页
    M_WIFI_CONFIG, // wifi配网页
    M_SETTING,
    M_ABOUT,
    M_OTA,
};

// 状态，初始化标签
enum
{
    S_FADE,      // 转场动画
    S_WINDOW,    // 弹窗初始化
    S_LAYER_IN,  // 层级初始化
    S_LAYER_OUT, // 层级初始化
    S_NONE,      // 直接选择页面
};

// 菜单结构体
typedef struct MENU
{
    const char *m_select;
} M_SELECT;

extern M_SELECT main_menu[];
extern M_SELECT main_menu_exp[];
extern M_SELECT switch_menu[];
extern M_SELECT usb_monitor_menu[];
extern M_SELECT usb_monitor_setting_menu[];
extern M_SELECT setting_menu[];
extern M_SELECT about_menu[];

// 磁贴变量
// 所有磁贴页面都使用同一套参数
#define TILE_B_FONT u8g2_font_helvB24_tr      // 磁贴大标题字体
#define TILE_S_FONT u8g2_font_wqy12_t_gb2312a // 磁贴小标题字体
#define TILE_B_TITLE_H 25                     // 磁贴大标题字体高度
#define TILE_S_TITLE_H 8                      // 磁贴小标题字体高度
#define TILE_ICON_H 48                        // 磁贴图标高度
#define TILE_ICON_W 48                        // 磁贴图标宽度
#define TILE_ICON_S 57                        // 磁贴图标间距
#define TILE_INDI_H 40                        // 磁贴大标题指示器高度
#define TILE_INDI_W 10                        // 磁贴大标题指示器宽度
#define TILE_INDI_S 57                        // 磁贴大标题指示器上边距

struct tile_t
{
    float title_y_calc;
    float title_y_trg_calc;
    int16_t temp;
    bool select_flag;
    float icon_x;
    float icon_x_trg;
    float icon_y;
    float icon_y_trg;
    float indi_x;
    float indi_x_trg;
    float title_y;
    float title_y_trg;
};
extern tile_t tile;

// 列表变量
// 默认参数
#define LIST_FONT u8g2_font_wqy12_t_gb2312a // 列表字体
#define LIST_TEXT_H 8                       // 列表每行文字字体的高度
#define LIST_LINE_H 16                      // 列表单行高度
#define LIST_TEXT_S 4                       // 列表每行文字的上边距，左边距和右边距，下边距由它和字体高度和行高度决定
#define LIST_BAR_W 5                        // 列表进度条宽度，需要是奇数，因为正中间有1像素宽度的线
#define LIST_BOX_R 0.5f                     // 列表选择框圆角

struct list_t
{
    uint8_t line_n = DISP_H / LIST_LINE_H;
    int16_t temp;
    bool loop;
    float y;
    float y_trg;
    float box_x;
    float box_x_trg;
    float box_y;
    float box_y_trg[UI_DEPTH];
    float bar_y;
    float bar_y_trg;
};
extern list_t list;

// USB监视器页面变量
#define USB_MONITOR_FONT u8g2_font_helvB24_tr      // 电压电流数字字体
#define USB_MONITOR_UNIT_FONT u8g2_font_helvB14_tr // 单位字体
#define USB_MONITOR_LIST_U_S 94                    // 列表上边距
#define USB_MONITOR_TEXT_BG_U_S 53                 // 文字背景框上边距
#define USB_MONITOR_TEXT_BG_H 33                   // 文字背景框高度
#define USB_MONITOR_PARAM 2                        // USB监视器设置数量

enum
{
    REFRESH_RATE,         // 数据刷新率Hz
    MONITOR_SERIAL_OUTPUT // 电压电流数据串口输出
};

struct usb_monitor_t
{
    int val;
    float text_bg_l;
    float text_bg_l_trg;
    uint8_t param[USB_MONITOR_PARAM];
};
extern usb_monitor_t usb_monitor;

// 选择框变量
// 默认参数
#define CHECK_BOX_L_S 95 // 选择框在每行的左边距
#define CHECK_BOX_U_S 2  // 选择框在每行的上边距
#define CHECK_BOX_F_W 12 // 选择框外框宽度
#define CHECK_BOX_F_H 12 // 选择框外框高度
#define CHECK_BOX_D_S 2  // 选择框里面的点距离外框的边距

struct check_box_t
{
    uint8_t *v;
    uint8_t *m;
    uint8_t *s;
    uint8_t *s_p;
};
extern check_box_t check_box;

// 弹窗变量
#define WIN_FONT u8g2_font_HelvetiPixel_tr     // 弹窗字体
#define WIN_H 32                               // 弹窗高度
#define WIN_W 102                              // 弹窗宽度
#define WIN_BAR_W 92                           // 弹窗进度条宽度
#define WIN_BAR_H 7                            // 弹窗进度条高度
#define WIN_Y -WIN_H - 2                       // 弹窗竖直方向出场起始位置
#define WIN_Y_TRG -WIN_H - 2                   // 弹窗竖直方向退场终止位置
#define WIN_MSG_H 48                           // 消息弹窗高度
#define WIN_MSG_W 120                          // 消息弹窗宽度
#define WIN_MSG_FONT u8g2_font_wqy12_t_gb2312a // 消息弹窗字体

struct win_t
{
    // uint8_t
    uint8_t *value;
    uint8_t max;
    uint8_t min;
    uint8_t step;

    MENU *bg;
    uint8_t index;
    char title[20];
    char sub_title[20];
    uint8_t select;
    uint8_t l = (DISP_W - WIN_W) / 2;
    uint8_t u = (DISP_H - WIN_H) / 2;
    uint8_t l_msg = (DISP_W - WIN_MSG_W) / 2;
    uint8_t u_msg = (DISP_H - WIN_MSG_H) / 2;
    float bar;
    float bar_trg;
    float y;
    float y_trg;
    float y_msg_trg;
    bool is_msg;
};
extern win_t win;

// 聚光灯变量
struct spot_t
{
    float l;
    float l_trg;
    float r;
    float r_trg;
    float u;
    float u_trg;
    float d;
    float d_trg;
};

extern spot_t spot;

#define USB_COUNT 4 // USB数量
struct usb_switch_t
{
    uint8_t switches[USB_COUNT] = {1, 1, 1, 1};
};
extern usb_switch_t usb_switch;

/**
 * @brief 初始化UI参数
 */
void ui_param_init();

/**
 * @brief 初始化UI
 */
void ui_init();

/**
 * @brief 初始化磁贴参数
 */
void tile_param_init();

/**
 * @brief UI处理函数，应在主循环中调用
 */
void ui_proc();

/**
 * @brief 初始化一个消息弹窗
 * @param title 弹窗主标题
 * @param sub_title 弹窗副标题
 */
void window_msg_init(const char *title, const char *sub_title);

/**
 * @brief 初始化一个可调数值的弹窗
 * @param title 弹窗标题
 * @param select 当前选择项
 * @param value 指向要修改的数值的指针
 * @param max 数值的最大值
 * @param min 数值的最小值
 * @param step 每次调整的步长
 * @param bg 背景菜单
 * @param index 当前菜单索引
 */
void window_value_init(const char *title, uint8_t select, uint8_t *value, uint8_t max, uint8_t min, uint8_t step, MENU *bg, uint8_t index);

#endif