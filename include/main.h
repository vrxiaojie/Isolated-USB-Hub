#ifndef MAIN_H_
#define MAIN_H_

// OLED变量
#define DISP_H 128 // 屏幕高度
#define DISP_W 128 // 屏幕宽度

// UI变量
#define UI_DEPTH 20  // 最深层级数
#define UI_MNUMB 100 // 菜单数量
#define UI_PARAM 16  // 参数数量
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
#endif