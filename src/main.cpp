#include <U8g2lib.h>
#include <Wire.h>
#include "RTOS_Task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ui.h"
#include "knob.h"
#include "oled.h"
#include "e2prom.h"

#define PWREN1 6
#define PWREN2 15
#define PWREN3 17
#define PWREN4 8
#define OVC1 7
#define OVC2 16
#define OVC3 18
#define OVC4 9

TaskHandle_t OVC_detect_Task_Handler = NULL;

void OVC_detect_Task(void *args)
{
  while (1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    if (digitalRead(OVC1) == LOW)
    {
      Serial.println("USB1过流");
    }
    if (digitalRead(OVC2) == LOW)
    {
      Serial.println("USB2过流");
    }
    if (digitalRead(OVC3) == LOW)
    {
      Serial.println("USB3过流");
    }
    if (digitalRead(OVC4) == LOW)
    {
      Serial.println("USB4过流");
    }
    delay(100); // 消抖处理
  }
}

void over_current_ISR()
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  vTaskNotifyGiveFromISR(OVC_detect_Task_Handler, &xHigherPriorityTaskWoken);
}

void setup()
{
  Serial.begin(115200);
  Wire.begin(SDA, SCL);
  eeprom_init();
  ui_param_init();
  set_usb_monitor_param();
  ui_init();
  oled_init();
  btn_init();
  tile_param_init(); // 默认进入主菜单ui.index = M_MAIN，需要初始化磁贴

  pinMode(OVC1, INPUT_PULLUP);
  pinMode(OVC2, INPUT_PULLUP);
  pinMode(OVC3, INPUT_PULLUP);
  pinMode(OVC4, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(OVC1), over_current_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(OVC2), over_current_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(OVC3), over_current_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(OVC4), over_current_ISR, FALLING);

  xTaskCreate(INA226_Task, "ina226", 1024 * 10, NULL, 3, &INA226_Task_Handle);
  xTaskCreate(btn_scan, "knob", 1024 * 20, NULL, 4, NULL);
  xTaskCreate(OVC_detect_Task, "OVC_Task", 1024 * 10, NULL, 3, &OVC_detect_Task_Handler);
}

void loop()
{
  ui_proc();
}
