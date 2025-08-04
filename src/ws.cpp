#include "ws.h"

// WebSocket 实例
WebSocketsServer webSocket = WebSocketsServer(81);

// ws回调
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    if (type == WStype_CONNECTED)
    {
        // Serial.println("Client connected");
    }
    else if (type == WStype_DISCONNECTED)
    {
        // Serial.println("Client disconnected");
    }
    else if (type == WStype_TEXT)
    {
        // Serial.printf("Received data from client: %s\n", payload);

        // 解析 JSON 命令 控制USB的开关
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (!error)
        {
            uint8_t usb_port = doc["usb_port"];
            uint8_t status = doc["status"];

            switch_ctrl(usb_port - 1, status);

            // Serial.printf("usb%d : %d", usb_port, status);
        }
    }
}