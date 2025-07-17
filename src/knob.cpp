#include <Arduino.h>
#include "knob.h"
#include "ui.h"
/************************************* 旋钮相关 *************************************/

btn_t btn;

// 编码器旋转判断
void knob_inter()
{
    btn.alv = digitalRead(AIO);
    btn.blv = digitalRead(BIO);
    if (!btn.flag && btn.alv == LOW)
    {
        btn.CW_1 = btn.blv;
        btn.flag = true;
    }
    if (btn.flag && btn.alv)
    {
        btn.CW_2 = !btn.blv;
        if (btn.CW_1 && btn.CW_2)
        {
            Serial.println("顺时针");
            btn.id = BTN_ID_CC;
            btn.pressed = true;
        }
        if (btn.CW_1 == false && btn.CW_2 == false)
        {
            Serial.println("逆时针");
            btn.id = BTN_ID_CW;
            btn.pressed = true;
        }
        btn.flag = false;
    }
}

void btn_init()
{
    // 初始化IO
    // TODO: 实装时不需要上拉输入
    pinMode(AIO, INPUT_PULLUP);
    pinMode(BIO, INPUT_PULLUP);
    pinMode(SW, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(AIO), knob_inter, CHANGE);
}