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

/**
 * @brief 初始化USB开关控制引脚
 */
void switch_pin_init();

/**
 * @brief 过流保护中断服务程序
 */
void over_current_ISR();

/**
 * @brief 控制指定的USB端口开关
 * @param n 要控制的开关编号 (SW1, SW2, SW3, SW4)
 * @param status 要设置的状态 (SW_ON or SW_OFF)
 */
void switch_ctrl(uint8_t n, uint8_t status);
#endif