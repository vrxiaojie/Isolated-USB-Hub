#ifndef KNOB_H_
#define KNOB_H_
// 按键ID
#define BTN_ID_CC 0 // 逆时针旋转
#define BTN_ID_CW 1 // 顺时针旋转
#define BTN_ID_SP 2 // 短按
#define BTN_ID_LP 3 // 长按

// 可按下旋钮引脚
#define AIO 40
#define BIO 39
#define SW 38

// 按键变量
#define BTN_PARAM_TIMES 2 // 由于uint8_t最大值可能不够，但它存储起来方便，这里放大两倍使用

struct btn_t
{
    uint8_t id;
    bool flag;
    bool pressed;
    bool CW_1;
    bool CW_2;
    bool val;
    bool val_last;
    bool alv;
    bool blv;
    long count;
};

extern btn_t btn;

/**
 * @brief 初始化旋钮编码器和按键的引脚
 */
void btn_init();
#endif