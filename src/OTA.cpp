#include "OTA.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Update.h>
#include "ui.h"
#include "e2prom.h"
// OTA更新信息URL
const char *json_url = "https://raw.gitcode.com/VRxiaojie/USBHUB-OTA/raw/main/version.json";

WiFiClientSecure client;

// 网络问题报错
void showNetworkErr()
{
    u8g2.clearBuffer();
    u8g2.drawUTF8(0, 16, "【网络连接错误】");
    u8g2.drawUTF8(0, 32, "请检查网络环境!");
    u8g2.drawUTF8(0, 48, "将在1秒后自动退出");
    u8g2.sendBuffer();
}

// 写入固件报错
void showWriteFirmwareErr()
{
    u8g2.clearBuffer();
    u8g2.drawUTF8(0, 16, "【升级失败】");
    u8g2.drawUTF8(0, 32, "固件写入出错!");
    u8g2.drawUTF8(0, 48, "将在1秒后自动退出");
    u8g2.sendBuffer();
}

// 固件验证报错
void showVerifyFirmwareErr()
{
    u8g2.clearBuffer();
    u8g2.drawUTF8(0, 16, "【固件验证失败】");
    u8g2.drawUTF8(0, 32, "错误码:");
    u8g2.drawUTF8(40, 32, String(Update.getError()).c_str());
    u8g2.drawUTF8(0, 48, "将在2秒后自动退出");
    u8g2.sendBuffer();
}

int getProgress(size_t progress, size_t total)
{
    static int lastPercent = -1;
    int percent = (progress * 100) / total;

    if (percent != lastPercent)
    {
        // Serial.printf("进度: %d%%\n", percent);
        lastPercent = percent;
    }
    return percent;
}

// 执行OTA更新的核心函数
bool performOTA(const char *url)
{
    // client 已经被设置为 insecure 模式了，所以这里不需要重复设置
    HTTPClient http;

    // *** 关键改动 2 (同样应用在这里) ***
    http.begin(client, url);

    const char *headerKeys[] = {"Location"};
    http.collectHeaders(headerKeys, 1); // 1 表示我们要收集的头的数量

    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_MOVED_PERMANENTLY || httpCode == HTTP_CODE_FOUND)
    {
        String newUrl = http.header("Location");
        // Serial.println("Redirecting to: " + newUrl);
        http.end();                         // 关闭旧连接
        http.begin(client, newUrl.c_str()); // 使用新 URL 开始新连接
        httpCode = http.GET();              // 再次发送请求
    }

    if (httpCode == HTTP_CODE_OK)
    {
        int contentLength = http.getSize();
        if (contentLength <= 0)
        {
            // Serial.println("Content-Length header missing or invalid. Cannot OTA.");
            showNetworkErr();
            delay(1000);
            http.end();
            return false;
        }

        if (!Update.begin(contentLength))
        {
            // Serial.println("Not enough space to begin OTA");
            showWriteFirmwareErr();
            delay(1000);
            // Update.printError(Serial);
            http.end();
            return false;
        }

        WiFiClient *stream = http.getStreamPtr();

        size_t written = 0;
        uint8_t buff[1024] = {0}; // 缓冲区
        unsigned long lastProgress = 0;

        u8g2.drawUTF8(0, 32, "开始写入固件");
        while (http.connected() && (written < contentLength))
        {
            size_t available = stream->available();

            if (available)
            {
                int readBytes = stream->readBytes(buff, min(available, sizeof(buff)));

                size_t bytesWritten = Update.write(buff, readBytes);

                if (bytesWritten != readBytes)
                {
                    // Serial.println("写入错误: 预期 " + String(readBytes) + " 字节，实际写入 " + String(bytesWritten) + " 字节");
                    showWriteFirmwareErr();
                    delay(1000);
                    Update.abort();
                    http.end();
                    return false;
                }

                written += bytesWritten;

                // 显示进度
                if (millis() - lastProgress > 100)
                {
                    int percent = getProgress(written, contentLength);
                    u8g2.clearBuffer();
                    u8g2.setFont(u8g2_font_wqy12_t_gb2312a);
                    u8g2.drawUTF8(0, 16, "【升级中,请勿断电】");
                    u8g2.drawUTF8(0, 32, "开始写入固件");
                    u8g2.setFont(u8g2_font_helvB24_tr);
                    if (percent < 10) // 一位数时保持居中
                        u8g2.setCursor(42, 80);
                    else if (percent < 100)
                        u8g2.setCursor(34, 80);
                    u8g2.printf("%d%%", percent);
                    u8g2.drawRFrame(20, 95, 88, 10, 1);                                      // 绘制进度条外框
                    u8g2.drawBox(20 + 2, 95 + 2, (float)percent / 100.0 * (88 - 4), 10 - 4); // 绘制进度条
                    u8g2.sendBuffer();

                    lastProgress = millis();
                }
            }
            else
            {
                delay(1); // 等待更多数据
            }

            // 喂狗，防止看门狗重启
            yield();
        }
        u8g2.setFont(u8g2_font_wqy12_t_gb2312a);
        if (written != contentLength)
        {
            // Serial.println("下载不完整: " + String(written) + "/" + String(contentLength));
            showNetworkErr();
            delay(1000);
            Update.abort();
            http.end();
            return false;
        }

        if (Update.end())
        {
            // Serial.println("固件验证通过，OTA升级完成");
            u8g2.clearBuffer();
            u8g2.drawUTF8(0, 16, "【OTA升级完成】");
            u8g2.drawUTF8(0, 32, "将在1秒后自动重启");
            u8g2.sendBuffer();
            delay(1000);
            http.end();
            return true;
        }
        else
        {
            showVerifyFirmwareErr();
            delay(2000);
            http.end();
            return false;
            // 详细错误信息
            // switch (Update.getError())
            // {
            // case UPDATE_ERROR_OK:
            //     Serial.println("没有错误");
            //     break;
            // case UPDATE_ERROR_WRITE:
            //     Serial.println("Flash写入错误");
            //     break;
            // case UPDATE_ERROR_ERASE:
            //     Serial.println("Flash擦除错误");
            //     break;
            // case UPDATE_ERROR_READ:
            //     Serial.println("Flash读取错误");
            //     break;
            // case UPDATE_ERROR_SPACE:
            //     Serial.println("空间不足");
            //     break;
            // case UPDATE_ERROR_SIZE:
            //     Serial.println("固件大小错误");
            //     break;
            // case UPDATE_ERROR_STREAM:
            //     Serial.println("数据流错误");
            //     break;
            // case UPDATE_ERROR_MD5:
            //     Serial.println("MD5校验失败");
            //     break;
            // case UPDATE_ERROR_MAGIC_BYTE:
            //     Serial.println("魔数验证失败");
            //     break;
            // case UPDATE_ERROR_ACTIVATE:
            //     Serial.println("激活失败");
            //     break;
            // case UPDATE_ERROR_NO_PARTITION:
            //     Serial.println("找不到更新分区");
            //     break;
            // case UPDATE_ERROR_BAD_ARGUMENT:
            //     Serial.println("参数错误");
            //     break;
            // case UPDATE_ERROR_ABORT:
            //     Serial.println("升级被中止");
            //     break;
            // default:
            //     Serial.println("未知错误");
            //     break;
            // }
        }
    }
    else
    {
        // Serial.printf("Failed to download firmware. HTTP code: %d\n", httpCode);
        showNetworkErr();
        delay(1000);
        return false;
    }
    http.end();
    return false;
}

// 检查并执行OTA更新
bool checkForOTA()
{
    u8g2.clearBuffer();
    u8g2.drawUTF8(0, 16, "检查更新中,请勿断电");
    u8g2.sendBuffer();
    client.setInsecure();

    HTTPClient http;

    http.begin(client, json_url);

    int httpCode = http.GET();
    if (httpCode > 0)
    {
        if (httpCode == HTTP_CODE_OK)
        {
            String payload = http.getString();

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (error)
            {
                u8g2.clearBuffer();
                u8g2.drawUTF8(0, 16, "【连接失败】");
                u8g2.drawUTF8(0, 32, "请检查网络连接");
                u8g2.drawUTF8(0, 48, "将在1秒后自动退出");
                u8g2.sendBuffer();
                delay(1000);
                http.end();
                return false;
            }

            const char *new_version = doc["version"];

            // 检查固件是否有更新
            if (strcmp(new_version, FIRMWARE_VERSION) > 0)
            {
                u8g2.clearBuffer();
                u8g2.drawUTF8(0, 16, "【有新固件】");
                u8g2.setCursor(0, 32);
                u8g2.printf("v%s", new_version);
                u8g2.drawUTF8(0, 48, "将在2秒后重启更新");
                u8g2.sendBuffer();
                delay(2000);
                return true;
            }
            else // 固件已经是最新
            {
                u8g2.clearBuffer();
                u8g2.drawUTF8(0, 16, "【当前已是最新版本】");
                u8g2.setCursor(0, 32);
                u8g2.printf("v%s", FIRMWARE_VERSION);
                u8g2.drawUTF8(0, 48, "将在2秒后自动退出");
                u8g2.sendBuffer();
                delay(2000);
                return false;
            }
        }
        else
        {
            showNetworkErr();
            delay(1000);
        }
    }
    else
    {
        showNetworkErr();
        delay(1000);
    }
    http.end();
    return false;
}

void checkOTAReady()
{
    if (EEPROM_read_ota_flag() != true)
        return;

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_wqy12_t_gb2312a);
    u8g2.setDrawColor(1);
    EEPROM_write_ota_flag(false);
    WiFi.begin();
    while (WiFi.status() != WL_CONNECTED)
    {
        u8g2.drawUTF8(0, 16, "连接WiFi中,请稍候...");
        u8g2.sendBuffer();
    }
    u8g2.clearBuffer();
    u8g2.drawUTF8(0, 16, "【升级中,请勿断电】");
    u8g2.sendBuffer();
    HTTPClient http;
    client.setInsecure();
    http.begin(client, json_url);

    int httpCode = http.GET();
    if (httpCode > 0)
    {
        if (httpCode == HTTP_CODE_OK)
        {
            String payload = http.getString();

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (error)
            {
                http.end();
                return;
            }
            const char *download_url = doc["download_url"];
            if (performOTA(download_url))
            {
                // Serial.println("restart..");
                ESP.restart();
            }
            else
            {
                return;
            }
        }
    }
}