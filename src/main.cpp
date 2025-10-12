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
#include "OTA.h"

void setup()
{
  Serial.begin(115200);
  Wire.begin(SDA, SCL);
  oled_init();

  encrypt_mac();
  // 如果未激活
  if (EEPROM_read_activation() == false)
  {
    uint8_t mac[6];
    char mac_str[13];
    esp_efuse_mac_get_default(mac);
    sprintf(mac_str, "%02X%02X%02X%02X%02X%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    u8g2.setFont(u8g2_font_wqy12_t_gb2312a);
    u8g2.drawUTF8(0, 16, "-----设备未激活-----");
    u8g2.drawUTF8(0, 32, "请用PC端激活软件激活,");
    u8g2.drawUTF8(0, 48, "每人免费激活一台设备.");
    u8g2.drawUTF8(0, 64, "激活多台设备的方法,");
    u8g2.drawUTF8(0, 80, "见激活工具的底部.");
    u8g2.drawUTF8(0, 96, "遇问题请在工程下留言");
    u8g2.setCursor(0, 112);
    u8g2.printf("MAC: %s", mac_str);
    u8g2.sendBuffer();
    activation_loop();
  }
  // 检查是否进入OTA更新模式
  checkOTAReady();

  EEPROM_read_ui_setting();
  EEPROM_read_wifi_setting();
  EEPROM_read_monitor_setting();
  ui_init();
  btn_init();
  switch_pin_init();
  tile_param_init(); // 默认进入主菜单ui.index = M_MAIN，需要初始化磁贴

  if (wifi.param[WIFI_AUTO_CONN]) // 开机自动连接WIFI
  {
    WiFi.begin();
  }
  xTaskCreate(INA226_Task, "ina226", 1024 * 10, NULL, 4, &INA226_Task_Handle);
  xTaskCreate(btn_scan, "knob", 1024 * 4, NULL, 5, NULL);
  xTaskCreate(OVC_detect_Task, "OVC_Task", 1024 * 4, NULL, 3, &OVC_detect_Task_Handler);
  xTaskCreate(WebSocket_Task, "ws", 20 * 1024, NULL, 4, &webSocketTaskHandle);
}

void loop()
{
  ui_proc();
}
