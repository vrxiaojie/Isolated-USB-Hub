#ifndef RTOS_TASK_H_
#define RTOS_TASK_H_
#include "FreeRTOS.h"
#include "task.h"
void INA226_Task(void *arg);

extern TaskHandle_t INA226_Task_Handle;

#endif