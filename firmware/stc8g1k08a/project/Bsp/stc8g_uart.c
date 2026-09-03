#include <stc8g_uart.h>

bit busy;
char wptr;
char rptr;
char buffer[16];

static void UartIsr() interrupt 4
{
	if (TI)
	{
		TI = 0; 
		busy = 0;
	}
	if (RI)
	{
		RI = 0; 
		if (((wptr + 1) & 0x0F) != rptr)
        {
            buffer[wptr++] = SBUF;
            wptr &= 0x0F;
        }
	} 
}

void UartInit(void)
{
	
	SCON = 0x50;
	TMOD = 0x00;
	TL1 = BRT;
	TH1 = BRT >> 8;
	TR1 = 1;
	AUXR &= 0xFE;  // 定时器1做波特率发生器
	AUXR |= 0x40;  // 1T模式
	wptr = 0;
	rptr = 0;
	busy = 0;
}

void UartSend(char dat)
{
	while (busy);
	busy = 1;
	SBUF = dat;
}


// 为printf提供底层支持的字符输出函数
char putchar (char dat)
{
    // 处理换行符：将 '\n' 转换为 '\r\n'
    if (dat == '\n')
        UartSend('\r');
    
    UartSend((char)dat);
    return dat;
}


// 判断是否有数据
bit UartHasData(void){
	return (rptr != wptr);
}
// 从串口读1个字节
char UartGetChar(void)
{
	char dat;
	if (rptr != wptr)
	{
		dat = buffer[rptr++];
		rptr &= 0x0F;
		return dat;
	}
	return 0;
}