/*必须将printf函数放到
	ES = 1;
    EA = 1;要不然没法用*/

#ifndef __STC8G_UART_H_
#define	__STC8G_UART_H_

#include <STC8G.H>
#include <stdio.h>  // 必须包含stdio.h
#include <INTRINS.H>

#define FOSC 11059200UL  // 11.0592M零误差晶振
#define BRT (65536 - FOSC / 115200 / 4)

void UartInit(void);	
void UartSend(char dat);

bit UartHasData(void);
// 读1个字节数据，从串口
char UartGetChar(void);	

#endif