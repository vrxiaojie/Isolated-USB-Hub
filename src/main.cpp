#include <U8g2lib.h>
#include <Wire.h>
#include "RTOS_Task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ui.h"
#include "knob.h"
#include "oled.h"
#include "e2prom.h"
#include "switch.h"

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
  switch_pin_init();
  tile_param_init(); // 默认进入主菜单ui.index = M_MAIN，需要初始化磁贴

  xTaskCreate(INA226_Task, "ina226", 1024 * 10, NULL, 3, &INA226_Task_Handle);
  xTaskCreate(btn_scan, "knob", 1024 * 20, NULL, 4, NULL);
  xTaskCreate(OVC_detect_Task, "OVC_Task", 1024 * 10, NULL, 3, &OVC_detect_Task_Handler);
}

void loop()
{
  ui_proc();
}
