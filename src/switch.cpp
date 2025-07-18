#include <Arduino.h>
#include "FreeRTOS.h"
#include "task.h"
#include "switch.h"
#include "ui.h"
#include "RTOS_Task.h"

void over_current_ISR()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(OVC_detect_Task_Handler, &xHigherPriorityTaskWoken);
}
