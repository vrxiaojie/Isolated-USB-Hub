#ifndef OTA_H_
#define OTA_H_
#define FIRMWARE_VERSION "1.3.0.1"
/**
 * @brief 检查是否有可用的OTA更新
 * @return true 如果有可用的更新
 * @return false 如果没有可用的更新
 */
bool checkForOTA();

/**
 * @brief 如果OTA更新准备就绪，则执行更新
 */
void checkOTAReady();
#endif