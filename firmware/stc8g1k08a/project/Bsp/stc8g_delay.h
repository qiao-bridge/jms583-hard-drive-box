#ifndef __STC8G_DELAY_H_
#define __STC8G_DELAY_H_

#include <STC8G.H>

// 主频定义，必须和实际一致
#define MAIN_FOSC 11059200UL

void delay_us(unsigned int us);  // 微秒级延时（大致）
void delay_ms(unsigned int ms);  // 毫秒级延时（精确）

#endif