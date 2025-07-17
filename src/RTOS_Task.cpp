#include "RTOS_Task.h"

#include "ina226_data.h"
TaskHandle_t INA226_Task_Handle = NULL;
void INA226_Task(void *arg)
{
    uint8_t addr[4] = {0x40, 0x41, 0x43, 0x44};
    for (uint8_t i = 0; i < 4; i++)
    {
        ina226_ctrl[i].begin();
        delay(10);
        if (ina226_ctrl[i].getManufacturerID() != 0x5449)
        {
            Serial.printf("无法找到地址为0x%x的INA226\n", ina226_ctrl[i].getAddress());
            ina226_data[i].init = false;
            continue;
        }
        ina226_data[i].init = true;
        ina226_ctrl[i].setMaxCurrentShunt(0.8, 0.1);  // 最大0.8A ，采样电阻0.1 ohm
        ina226_ctrl[i].setAverage(INA226_64_SAMPLES); // 64 samples
    }

    for (;;)
    {
        uint32_t idx;
        // 当获取到通知(进入数据显示页时)才开始工作
        xTaskNotifyWait(0, 0, &idx, portMAX_DELAY);
        switch (idx)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            if (ina226_data[idx].init)
            {
                ina226_data[idx].busVoltage = ina226_ctrl[idx].getBusVoltage();
                ina226_data[idx].current_mA = ina226_ctrl[idx].getCurrent_mA();
                ina226_data[idx].power_mW = ina226_data[idx].busVoltage * ina226_data[idx].current_mA;
            }
            break;
        case 4:
            // TODO:  自动轮换显示USB1~4数据
            break;
        }
        delay(100);
    }
}

#include "knob.h"
#include "ui.h"
// 扫描编码器按键按下的任务
void btn_scan(void *args)
{
    while (1)
    {
        btn.val = digitalRead(SW);
        if (btn.val != btn.val_last)
        {
            btn.val_last = btn.val;
            delay(ui.param[BTN_SPT] * BTN_PARAM_TIMES);
            btn.val = digitalRead(SW);
            if (btn.val == LOW)
            {
                btn.count = 0;
                while (!digitalRead(SW))
                {
                    btn.count++;
                    delay(1);
                }
                if (btn.count < ui.param[BTN_LPT] * BTN_PARAM_TIMES)
                {
                    btn.id = BTN_ID_SP;
                    Serial.println("短按");
                }
                else
                {
                    btn.id = BTN_ID_LP;
                    Serial.println("长按");
                }
                btn.pressed = true;
            }
        }

        delay(5);
    }
}
