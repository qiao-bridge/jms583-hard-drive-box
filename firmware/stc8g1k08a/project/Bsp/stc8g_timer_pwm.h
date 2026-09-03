#ifndef __STC8G_TIMER_PWM_H_
#define __STC8G_TIMER_PWM_H_

#include <STC8G.H>
#include "stc8g_gpio.h"

void timer0_pwm_init(unsigned char pwm_port, unsigned char pwm_pin); // 定时器0 PWM初始化
void timer0_pwm_setfreq(unsigned int freq); // 设置PWM频率（Hz）
void timer0_pwm_setduty(unsigned char duty); // 设置占空比（0~100%）
void timer0_pwm_enable(bit en);           // 使能/失能PWM

#endif