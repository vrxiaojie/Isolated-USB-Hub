#ifndef WS_H_
#define WS_H_
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "switch.h"
extern WebSocketsServer webSocket;

/**
 * @brief WebSocket事件处理函数
 *
 * @param num 客户端编号
 * @param type 事件类型 (e.g., WStype_DISCONNECTED, WStype_CONNECTED, WStype_TEXT)
 * @param payload 接收到的数据负载
 * @param length 数据负载的长度
 */
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
#endif