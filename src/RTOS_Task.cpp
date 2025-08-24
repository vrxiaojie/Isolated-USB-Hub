#include "RTOS_Task.h"
#include "ina226_data.h"
#include "knob.h"
#include "ui.h"
#include "switch.h"

static TimerHandle_t INA226_timer = NULL;
TaskHandle_t INA226_Task_Handle = NULL;
// INA226定时刷新数据回调函数
void INA226TimerCallback(TimerHandle_t xTimer)
{
    xTimerChangePeriod(xTimer, 1000 / usb_monitor.param[REFRESH_RATE], 0);
    xTaskNotifyGive(INA226_Task_Handle);
}

void INA226_Task(void *arg)
{
    INA226_timer = xTimerCreate(
        "INA226_timer",
        100,    // 100ms
        pdTRUE, // 自动重载
        NULL,
        INA226TimerCallback);
    if (INA226_timer != NULL)
    {
        xTimerStart(INA226_timer, 0);
    }

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
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == pdTRUE)
        {
            if (usb_monitor.param[MONITOR_SERIAL_OUTPUT])
                Serial.print("USB(volt,curr,power|ch1,2,3,4):"); // 分4个通道，每通道按序显示电压电流功率
            for (uint8_t idx = 0; idx < 4; idx++)
            {
                if (ina226_data[idx].init)
                {
                    ina226_data[idx].busVoltage = ina226_ctrl[idx].getBusVoltage();
                    ina226_data[idx].current_mA = ina226_ctrl[idx].getCurrent_mA();
                    ina226_data[idx].power_mW = ina226_data[idx].busVoltage * ina226_data[idx].current_mA;
                }
                if (usb_monitor.param[MONITOR_SERIAL_OUTPUT]) // 串口输出电压电流
                {
                    Serial.printf("%.2f,%.1f,%.1f,", ina226_data[idx].busVoltage,
                                  ina226_data[idx].current_mA, ina226_data[idx].power_mW);
                }
            }
            if (usb_monitor.param[MONITOR_SERIAL_OUTPUT])
                Serial.println();
        }
    }
}

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
                }
                else
                {
                    btn.id = BTN_ID_LP;
                }
                btn.pressed = true;
            }
        }

        delay(5);
    }
}

TaskHandle_t OVC_detect_Task_Handler = NULL;
// 检测USB端口过流任务
void OVC_detect_Task(void *args)
{
    pinMode(OVC1, INPUT);
    pinMode(OVC2, INPUT);
    pinMode(OVC3, INPUT);
    pinMode(OVC4, INPUT);
    attachInterrupt(digitalPinToInterrupt(OVC1), over_current_ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(OVC2), over_current_ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(OVC3), over_current_ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(OVC4), over_current_ISR, FALLING);

    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (digitalRead(OVC1) == LOW)
        {
            // 过流后立即切断该路供电，需手动恢复
            switch_ctrl(SW1, LOW);
            window_msg_init((char*)"USB1", (char*)"过流保护");
            // Serial.println("USB1过流");
        }
        if (digitalRead(OVC2) == LOW)
        {
            switch_ctrl(SW2, LOW);
            window_msg_init((char*)"USB2", (char*)"过流保护");
            // Serial.println("USB2过流");
        }
        if (digitalRead(OVC3) == LOW)
        {
            switch_ctrl(SW3, LOW);
            window_msg_init((char*)"USB3", (char*)"过流保护");
            // Serial.println("USB3过流");
        }
        if (digitalRead(OVC4) == LOW)
        {
            switch_ctrl(SW4, LOW);
            window_msg_init((char*)"USB4", (char*)"过流保护");
            // Serial.println("USB4过流");
        }
        delay(100);
    }
}

#include "WiFi.h"
#include "ws.h"
TaskHandle_t webSocketTaskHandle = NULL;
static TimerHandle_t wsTimer = NULL;
bool wifi_connected = false;   // 本地的wifi连接状态变量
bool clientsConnected = false; // 是否有客户端连接的变量
// 定时器回调 - 用于定期广播数据
void wsTimerCallback(TimerHandle_t xTimer)
{
    static uint8_t cnt = 0;
    cnt++;
    // 检查是否有客户端连接
    bool hasClients = false;
    for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
    {
        if (webSocket.clientIsConnected(i))
        {
            hasClients = true;
            break;
        }
    }
    clientsConnected = hasClients;
    // 每5s检查一次wifi连接情况
    if (cnt >= 5)
    {
        cnt = 0;
        if (WiFi.status() == WL_CONNECTED)
        {
            wifi_connected = true;
        }
        else
        {
            wifi_connected = false;
        }
    }
    // 仅当有客户端连接且wifi连接时，才发送数据
    if (clientsConnected && wifi_connected && webSocketTaskHandle != NULL)
    {
        // 通知WebSocket任务发送数据
        xTaskNotifyGive(webSocketTaskHandle);
    }
}

void WebSocket_Task(void *args)
{
    WiFi.begin();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent); // 设置 WebSocket 事件回调
    webSocket.enableHeartbeat(15000, 3000, 15);

    wsTimer = xTimerCreate(
        "wsTimer",
        1000,   // 1秒
        pdTRUE, // 自动重载
        NULL,
        wsTimerCallback);
    if (wsTimer != NULL)
    {
        xTimerStart(wsTimer, 0);
    }
    while (1)
    {
        webSocket.loop(); // 处理 WebSocket 事件
        uint32_t notificationValue = 0;
        if (ulTaskNotifyTake(pdTRUE, 0) == pdTRUE)
        {
            for (uint8_t i = 0; i < 4; i++)
            {
                // 构造 JSON 数据包
                String jsonData = "{\"usb_port\": " + String(i + 1) + ", \"voltage\": " + String(ina226_data[i].busVoltage) + ", \"current\": " + String(ina226_data[i].current_mA) + ", \"power\": " + String(ina226_data[i].power_mW) + ", \"status\": " + String(usb_switch.switches[i] ? "true" : "false") + "}";
                webSocket.broadcastTXT(jsonData); // 向所有客户端广播数据
            }
        }
        delay(1);
    }
}
