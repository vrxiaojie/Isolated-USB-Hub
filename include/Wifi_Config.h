#ifndef WIFI_CONFIG_H_
#define WIFI_CONFIG_H_

#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <ESPmDNS.h> //用于设备域名 MDNS.begin("esp32")

enum
{
    WIFI_AUTO_CONN,
    WIFI_DISABLE_ON_SLEEP,
};
#define WIFI_PARAM 2
extern const char *HOST_NAME; // 设置设备名
extern int connectTimeOut_s;  // WiFi连接超时时间，单位秒
struct wifi_t
{
    bool connectfailed = false;
    bool wifi_power = false;
    uint8_t param[WIFI_PARAM] = {0};
};
extern wifi_t wifi;

/**
 * @brief 检测WiFi是否已经连接
 * @param reConnect 如果WiFi未连接，是否尝试重新连接
 */
void checkConnect(bool reConnect);

/**
 * @brief 删除保存的WiFi信息
 * @return true 如果成功删除
 * @return false 如果删除失败
 */
bool restoreWiFi();

/**
 * @brief 在AP配网模式下，处理DNS和HTTP请求
 */
void checkDNS_HTTP();

/**
 * @brief 连接到WiFi
 * @param timeOut_s 连接超时时间（秒）
 * @return true 如果连接成功
 * @return false 如果连接失败
 */
bool connectToWiFi(int timeOut_s);

/**
 * @brief 处理Web服务器的根路径("/")请求，显示配网页面
 */
void handleRoot();

/**
 * @brief 处理从配网页面提交的WiFi凭证
 */
void handleConfigWifi();

/**
 * @brief 处理404 Not Found错误，重定向到根页面
 */
void handleNotFound();

/**
 * @brief 初始化Soft AP（接入点）模式
 */
void initSoftAP();

/**
 * @brief 初始化DNS服务器，用于强制门户
 */
void initDNS();

/**
 * @brief 初始化WebServer，设置URL处理函数
 */
void initWebServer();

/**
 * @brief 扫描附近的WiFi网络
 * @param n 扫描到的网络数量
 * @return true 如果扫描到网络
 * @return false 如果没有扫描到网络
 */
bool scanWiFi(int n);

/**
 * @brief 配置WiFi连接，包括启动AP模式和Web服务器进行配网
 */
void wifiConfig();

#endif