#ifndef OLED_H_
#define OLED_H_
#include <U8g2lib.h>
// OLED变量
#define SCLK 11
#define SDATA 12
#define DC 46
#define CS 10
#define RST 3

#define DISP_H 128 // 屏幕高度
#define DISP_W 128 // 屏幕宽度

extern U8G2_SH1107_SEEED_128X128_F_4W_HW_SPI u8g2;
extern uint8_t *buf_ptr;
extern uint16_t buf_len;

void oled_init();

#endif