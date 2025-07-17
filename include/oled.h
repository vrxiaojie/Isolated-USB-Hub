#ifndef OLED_H_
#define OLED_H_
#include <U8g2lib.h>
// OLED变量
#define SDA 8
#define SCL 18
#define RST U8X8_PIN_NONE

#define DISP_H 128 // 屏幕高度
#define DISP_W 128 // 屏幕宽度

extern U8G2_SH1107_SEEED_128X128_F_HW_I2C u8g2;
extern uint8_t *buf_ptr;
extern uint16_t buf_len;

void oled_init();

#endif