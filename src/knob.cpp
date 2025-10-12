#include <Arduino.h>
#include "knob.h"
#include "ui.h"
/************************************* 旋钮相关 *************************************/

btn_t btn;

// 编码器旋转判断
static void knob_inter()
{
    static unsigned long lastInterruptTime = 0;
    unsigned long interruptTime = millis();

    if (interruptTime - lastInterruptTime > 5)
    { // Debounce
        if (digitalRead(BIO) == digitalRead(AIO))
        {
            btn.id = BTN_ID_CC;
            btn.pressed = true;
        }
        else
        {
            btn.id = BTN_ID_CW;
            btn.pressed = true;
        }
    }
    lastInterruptTime = interruptTime;
}

void btn_init()
{
    // 初始化IO
    pinMode(AIO, INPUT);
    pinMode(BIO, INPUT);
    pinMode(SW, INPUT);
    attachInterrupt(digitalPinToInterrupt(AIO), knob_inter, CHANGE);
}