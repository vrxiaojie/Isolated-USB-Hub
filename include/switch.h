#ifndef SWITCH_H_
#define SWITCH_H_

#define PWREN1 6
#define PWREN2 15
#define PWREN3 17
#define PWREN4 8
#define OVC1 7
#define OVC2 16
#define OVC3 18
#define OVC4 9

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