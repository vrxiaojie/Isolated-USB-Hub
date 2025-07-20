#include "oled.h"
#include <U8g2lib.h>
#include "ui.h"
#include <SPI.h>

U8G2_SH1107_SEEED_128X128_F_4W_HW_SPI u8g2(U8G2_R0, CS, DC, RST);

uint8_t *buf_ptr; // 指向屏幕缓冲的指针
uint16_t buf_len; // 缓冲长度

// OLED初始化函数
void oled_init()
{
    SPI.begin(SCLK, -1, SDATA, CS);
    u8g2.begin();
    u8g2.setBusClock(2000000);
    u8g2.setContrast(ui.param[DISP_BRI]);
    buf_ptr = u8g2.getBufferPtr();
    buf_len = 8 * u8g2.getBufferTileHeight() * u8g2.getBufferTileWidth();
}