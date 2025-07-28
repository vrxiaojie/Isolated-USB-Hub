#ifndef SWITCH_H_
#define SWITCH_H_

#define PWREN1 8
#define PWREN2 17
#define PWREN3 15
#define PWREN4 6
#define OVC1 9
#define OVC2 18
#define OVC3 16
#define OVC4 7

enum
{
    SW1,
    SW2,
    SW3,
    SW4
};
enum
{
    SW_OFF,
    SW_ON
};

void switch_pin_init();
void over_current_ISR();
void switch_ctrl(uint8_t n, uint8_t status);
#endif