#ifndef __STC8G_GPIO_H_
#define	__STC8G_GPIO_H_

#include <STC8G.H>
#include <INTRINS.H>

/* 使用P12的方式去表示p12引脚（p34引脚），直接读取或者赋值,对引脚进行操作 */

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
bit stc8g_gpio_init(unsigned char port, unsigned char pin, unsigned char mode);

bit stc8g_gpio_set_level(unsigned char port, unsigned char pin, bit level);


#endif

/*
初始化模式	写1的效果	写0的效果	适用场景
准双向口（默认）	释放引脚，内部弱上拉输出高电平（驱动能力弱，~100uA）	强拉低引脚（灌电流强，~20mA）	按键检测、普通IO
推挽输出	强推高电平（驱动能力强，~20mA）	强拉低电平	LED、PWM输出、串口TX
高阻输入	写1/0都无效，引脚浮空	写1/0都无效，引脚浮空	ADC采样、高频输入
开漏输出	高阻态（需外接上拉才为高）	强拉低电平	I2C总线、电平匹配
*/