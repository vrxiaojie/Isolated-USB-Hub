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

void switch_pin_init()
{
    pinMode(PWREN1, OUTPUT_OPEN_DRAIN);
    pinMode(PWREN2, OUTPUT_OPEN_DRAIN);
    pinMode(PWREN3, OUTPUT_OPEN_DRAIN);
    pinMode(PWREN4, OUTPUT_OPEN_DRAIN);
    digitalWrite(PWREN1, HIGH);
    digitalWrite(PWREN2, HIGH);
    digitalWrite(PWREN3, HIGH);
    digitalWrite(PWREN4, HIGH);
}

void switch_ctrl(uint8_t n, uint8_t status)
{
    usb_switch.switches[n] = status;
    switch (n)
    {
    case SW1:
        digitalWrite(PWREN1, status);
        break;
    case SW2:
        digitalWrite(PWREN2, status);
        break;
    case SW3:
        digitalWrite(PWREN3, status);
        break;
    case SW4:
        digitalWrite(PWREN4, status);
        break;
    }
}
