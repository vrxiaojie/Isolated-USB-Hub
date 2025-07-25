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
#include "ina226_data.h"
#include "Wifi_Config.h"
#include "encrypt.h"

void setup()
{
  Serial.begin(115200);
  Wire.begin(SDA, SCL);
  send_mac();
  encrypt_mac();
  while (1)
  {
    if (Serial.available())
    {
      String received_data = Serial.readString(); // 接收电脑端发送的SHA256哈希数据
      received_data.trim();                       // 去除可能的换行符
      // 打印接收到的数据
      Serial.print("Received Encrypted Data (SHA256): ");
      Serial.println(received_data);

      check_encrypted_mac(received_data);
    }
  }
  EEPROM_read_ui_setting();
  EEPROM_read_wifi_setting();
  set_usb_monitor_param();
  ui_init();
  oled_init();
  btn_init();
  switch_pin_init();
  tile_param_init(); // 默认进入主菜单ui.index = M_MAIN，需要初始化磁贴

  if (wifi.param[WIFI_AUTO_CONN]) // 开机自动连接WIFI
  {
    WiFi.begin();
  }
  xTaskCreate(INA226_Task, "ina226", 1024 * 10, NULL, 4, &INA226_Task_Handle);
  xTaskCreate(btn_scan, "knob", 1024 * 20, NULL, 5, NULL);
  xTaskCreate(OVC_detect_Task, "OVC_Task", 1024 * 10, NULL, 3, &OVC_detect_Task_Handler);
  xTaskCreate(WebSocket_Task, "ws", 20 * 1024, NULL, 4, &webSocketTaskHandle);
}

void loop()
{
  ui_proc();
}
