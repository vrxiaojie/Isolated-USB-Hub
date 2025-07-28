#ifndef RTOS_TASK_H_
#define RTOS_TASK_H_
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
void INA226_Task(void *arg);
void btn_scan(void *args);
void OVC_detect_Task(void *args);
void WebSocket_Task(void *args);

extern TaskHandle_t INA226_Task_Handle;
extern TaskHandle_t OVC_detect_Task_Handler;
extern TaskHandle_t webSocketTaskHandle;

#endif