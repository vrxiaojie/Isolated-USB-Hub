#include "OTA.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Update.h>

// OTA更新信息URL
const char *json_url = "https://raw.githubusercontent.com/vrxiaojie/USBHub-OTA/refs/heads/main/version.json";

WiFiClientSecure client;

void printProgress(size_t progress, size_t total)
{
    static int lastPercent = -1;
    int percent = (progress * 100) / total;

    if (percent != lastPercent)
    {
        Serial.printf("进度: %d%%\n", percent);
        lastPercent = percent;
    }
}

// 执行OTA更新的核心函数
bool performOTA(const char *url)
{
    // client 已经被设置为 insecure 模式了，所以这里不需要重复设置
    HTTPClient http;

    // *** 关键改动 2 (同样应用在这里) ***
    http.begin(client, url);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        int contentLength = http.getSize();
        if (contentLength <= 0)
        {
            Serial.println("Content-Length header missing or invalid. Cannot OTA.");
            http.end();
            return false;
        }

        if (!Update.begin(contentLength))
        {
            Serial.println("Not enough space to begin OTA");
            Update.printError(Serial);
            http.end();
            return false;
        }

        Serial.println("Starting firmware update (insecurely)...");

        WiFiClient *stream = http.getStreamPtr();

        size_t written = 0;
        uint8_t buff[512] = {0}; // 使用较小的缓冲区以节省内存
        unsigned long lastProgress = 0;

        Serial.println("开始写入固件...");

        while (http.connected() && (written < contentLength))
        {
            size_t available = stream->available();

            if (available)
            {
                int readBytes = stream->readBytes(buff, min(available, sizeof(buff)));

                size_t bytesWritten = Update.write(buff, readBytes);

                if (bytesWritten != readBytes)
                {
                    Serial.println("写入错误: 预期 " + String(readBytes) + " 字节，实际写入 " + String(bytesWritten) + " 字节");
                    Update.abort();
                    http.end();
                    return false;
                }

                written += bytesWritten;

                // 显示进度（每2秒或每10%显示一次）
                if (millis() - lastProgress > 2000)
                {
                    printProgress(written, contentLength);
                    lastProgress = millis();
                }
            }
            else
            {
                delay(10); // 等待更多数据
            }

            // 喂狗，防止看门狗重启
            yield();
        }

        Serial.println(); // 换行

        if (written != contentLength)
        {
            Serial.println("下载不完整: " + String(written) + "/" + String(contentLength));
            Update.abort();
            http.end();
            return false;
        }

        if (Update.end(true))
        {
            Serial.println("固件验证通过，OTA升级完成");
            http.end();
            return true;
        }
        else
        {
            Serial.println("固件验证失败");
            Serial.println("错误代码: " + String(Update.getError()));

            // 详细错误信息
            switch (Update.getError())
            {
            case UPDATE_ERROR_OK:
                Serial.println("没有错误");
                break;
            case UPDATE_ERROR_WRITE:
                Serial.println("Flash写入错误");
                break;
            case UPDATE_ERROR_ERASE:
                Serial.println("Flash擦除错误");
                break;
            case UPDATE_ERROR_READ:
                Serial.println("Flash读取错误");
                break;
            case UPDATE_ERROR_SPACE:
                Serial.println("空间不足");
                break;
            case UPDATE_ERROR_SIZE:
                Serial.println("固件大小错误");
                break;
            case UPDATE_ERROR_STREAM:
                Serial.println("数据流错误");
                break;
            case UPDATE_ERROR_MD5:
                Serial.println("MD5校验失败");
                break;
            case UPDATE_ERROR_MAGIC_BYTE:
                Serial.println("魔数验证失败");
                break;
            case UPDATE_ERROR_ACTIVATE:
                Serial.println("激活失败");
                break;
            case UPDATE_ERROR_NO_PARTITION:
                Serial.println("找不到更新分区");
                break;
            case UPDATE_ERROR_BAD_ARGUMENT:
                Serial.println("参数错误");
                break;
            case UPDATE_ERROR_ABORT:
                Serial.println("升级被中止");
                break;
            default:
                Serial.println("未知错误");
                break;
            }
        }
    }
    else
    {
        Serial.printf("Failed to download firmware. HTTP code: %d\n", httpCode);
    }
    http.end();
}

// 检查并执行OTA更新
void checkForOTA()
{
    Serial.println("Checking for new firmware...");

    client.setInsecure();

    HTTPClient http;

    http.begin(client, json_url);

    int httpCode = http.GET();
    if (httpCode > 0)
    {
        if (httpCode == HTTP_CODE_OK)
        {
            String payload = http.getString();
            Serial.println("Successfully fetched JSON file (insecurely):");
            Serial.println(payload);

            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, payload);

            if (error)
            {
                Serial.print("deserializeJson() failed: ");
                Serial.println(error.c_str());
                http.end();
                return;
            }

            const char *new_version = doc["version"];
            Serial.print("Current firmware version: ");
            Serial.println(FIRMWARE_VERSION);
            Serial.print("Server firmware version: ");
            Serial.println(new_version);

            if (strcmp(new_version, FIRMWARE_VERSION) > 0)
            {
                Serial.println("New firmware available. Starting OTA...");
                const char *download_url = doc["download_url"];
                if (performOTA(download_url))
                {
                    ESP.restart();
                }
            }
            else
            {
                Serial.println("Firmware is up to date.");
            }
        }
        else
        {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
    }
    else
    {
        Serial.printf("[HTTP] Unable to connect to server\n");
    }
    http.end();
}
