#ifndef WS_H_
#define WS_H_
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "switch.h"
extern WebSocketsServer webSocket;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
#endif