#include <stc8g_gpio.h>


/**
 * @brief STC8G系列GPIO引脚初始化函数
 * @note  仅适配STC8G1K08系列，该芯片仅开放P1、P3、P5三个IO口
 * @param port  端口号：仅支持1(P1口)、3(P3口)、5(P5口)，其他值返回初始化失败
 * @param pin   引脚号：0~7，超出范围返回初始化失败
 * @param mode  GPIO工作模式，对应STC硬件寄存器定义：
 *              - 0：准双向口（M1=0, M0=0）：传统8051模式，内部弱上拉(~100uA)，适合按键输入、普通IO
 *              - 1：推挽输出（M1=0, M0=1）：强驱动(~20mA)，适合LED、PWM输出、串口TX
 *              - 2：高阻输入（M1=1, M0=0）：无内部上拉，适合ADC采样、高频信号输入
 *              - 3：开漏输出（M1=1, M0=1）：需外接上拉电阻，适合I2C总线、电平匹配
 * @return bit  初始化结果：1=成功，0=参数非法（端口不支持/引脚超范围）
 */
bit stc8g_gpio_init(unsigned char port, unsigned char pin, unsigned char mode){
	
	unsigned char mask = 0x01 << pin;
	
	if(pin >7) return 0;
	switch(mode){
		case 0://准双向口: M1=0, M0=0
			if(port == 1) 		{ P1M1 &= ~mask; P1M0 &= ~mask;}
			else if(port == 3)  { P3M1 &= ~mask; P3M0 &= ~mask;}
			else if(port == 5)  { P5M1 &= ~mask; P5M0 &= ~mask;}
			else return 0;// STC8G1K08 只有 P1/P3/P5
			break;
		case 1:// 推挽输出: M1=0, M0=1
			if(port == 1) 		{ P1M1 &= ~mask; P1M0|=mask;}
			else if(port == 3)  { P3M1 &= ~mask; P3M0|=mask;}
			else if(port == 5)  { P5M1 &= ~mask; P5M0|=mask;}
			else return 0;
			break;
		case 2:// 高阻输入: M1=1, M0=0
			if(port == 1) 		{ P1M1 |=mask; P1M0 &= ~mask;}
			else if(port == 3)  { P3M1 |=mask; P3M0 &= ~mask;}
			else if(port == 5)  { P5M1 |=mask; P5M0 &= ~mask;}
			else return 0;
			break;
		default://开漏输出: M1=1, M0=1
			if(port == 1) 		{ P1M1 |=mask; P1M0|=mask;}
			else if(port == 3)  { P3M1 |=mask; P3M0|=mask;}
			else if(port == 5)  { P5M1 |=mask; P5M0|=mask;}
			else return 0;
			break;
	}
	return 1; //初始化成功
}


/**
 * @brief 设置指定GPIO引脚的电平状态
 * @note  操作逻辑：直接读写端口输出寄存器Px，位操作为原子指令，可在中断中安全调用
 */
bit stc8g_gpio_set_level(unsigned char port, unsigned char pin, bit level)
{    
    switch(port){
        case 3:  // P3端口，你之前的逐个判断pin的写法完全保留
            if(pin == 0)      P30 = level;
            else if(pin == 1) P31 = level;
            else if(pin == 2) P32 = level;
            else if(pin == 3) P33 = level;  // 直接赋值bit变量，完全合法
            else return 1;
            break;
        case 5:
            if(pin == 4)      P54 = level;
            else if(pin == 5) P55 = level;
            else return 1;
            break;
        default:
            return 1;
    }
    return 0;  // 成功返回1（和你之前的返回值逻辑一致，不用改调用代码）
}
