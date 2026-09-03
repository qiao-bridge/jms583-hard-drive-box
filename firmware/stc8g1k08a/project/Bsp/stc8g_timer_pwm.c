#include "stc8g_timer_pwm.h"

// 静态变量：记录当前IO电平（0=低，1=高）
static volatile unsigned char pwm_current_level = 0;

// PWM全局配置（volatile防止编译器优化，中断里用的变量必须加）
volatile unsigned char pwm_duty = 50;    // 占空比0~100%
volatile unsigned int pwm_freq = 21600; // PWM频率，默认21.6kHz（适配4线风扇）
volatile bit pwm_en = 0;                 // PWM使能开关

// 定时器0初值缓存（双缓冲，避免中断里改值产生毛刺）
volatile unsigned int pwm_high_th = 0;  // 高电平定时初值（高8位）
volatile unsigned int pwm_high_tl = 0;  // 高电平定时初值（低8位）
volatile unsigned int pwm_low_th = 0;   // 低电平定时初值（高8位）
volatile unsigned int pwm_low_tl = 0;   // 低电平定时初值（低8位）

/**
 * 定时器0 PWM初始化
 * 配置：16位模式，1分频（11.0592MHz计数频率），开中断
 */
void timer0_pwm_init(unsigned char pwm_port, unsigned char pwm_pin)
{
    // 1. IO配置：推挽输出（和你之前的stc8g_gpio_init一致）
    stc8g_gpio_init(pwm_port, pwm_pin, 1);
	P55=0;
	
    // 2. 定时器模式配置：模式1（16位定时器，不自动重装）
    TMOD &= 0xF0; // 清定时器0的模式位（高4位留给定时器1）
    TMOD |= 0x01; // M1=0, M0=1 → 模式1

    // 3. 分频配置：1分频（11.0592MHz计数频率，对应PCA的1T模式）
    AUXR |= 0x80;  // T0x12=1 → 1分频，计数周期=1/11.0592M≈0.0904us
    // 如果要12分频（和传统8051一致），改成 AUXR &= ~0x80;

    // 4. 初始频率/占空比计算（默认21.6kHz，50%占空比）
    timer0_pwm_setfreq(pwm_freq);
    timer0_pwm_setduty(pwm_duty);

    // 5. 中断配置
    ET0 = 1; // 开定时器0中断
    EA  = 1; // 开总中断

    // 6. 默认不启动，等enable后再开
    TR0 = 0;
}

/**
 * 设置PWM频率（Hz）
 * 公式推导：
 * 计数频率 f_count = 11.0592MHz / 分频系数（这里分频=1）
 * 计数周期 t_count = 1 / f_count ≈ 0.0904us
 * PWM周期 T = 1/freq
 * 高电平时间 t_high = T * duty/100
 * 低电平时间 t_low = T * (100-duty)/100
 * 高电平计数次数 N_high = t_high / t_count
 * 低电平计数次数 N_low = t_low / t_count
 * 定时初值 = 65536 - 计数次数
 */
void timer0_pwm_setfreq(unsigned int freq)
{
    unsigned long period_us;      // PWM周期（微秒）
    unsigned long high_us, low_us; // 高低电平持续时间（微秒）
    unsigned int high_cnt, low_cnt; // 高低电平计数次数
    unsigned int duty = pwm_duty; // 读取当前占空比

    if(freq == 0) freq = 1; // 防止除零
    period_us = 1000000UL / freq; // 周期转微秒

    // 计算高低电平时间
    high_us = period_us * duty / 100;
    low_us  = period_us * (100 - duty) / 100;

    // 转成计数次数（1分频下，计数周期≈0.0904us）
    high_cnt = (unsigned int)(high_us / 0.090422455); // 1/11.0592e6 *1e6
    low_cnt  = (unsigned int)(low_us  / 0.090422455);


    // 双缓冲：先计算好初值，再赋值给全局变量，避免中断里读到半成品
    pwm_high_th = (65536 - high_cnt) >> 8; // 高8位
    pwm_high_tl = (65536 - high_cnt) & 0xFF; // 低8位
    pwm_low_th  = (65536 - low_cnt) >> 8;
    pwm_low_tl  = (65536 - low_cnt) & 0xFF;
}

/**
 * 设置占空比（0~100%）
 * 双缓冲逻辑：只改占空比变量，下次setfreq时自动生效，避免毛刺
 */
void timer0_pwm_setduty(unsigned char duty)
{
	//软件翻转IO的方式做不到绝对的0%/100%，必须关定时器直接拉IO，不然哪怕计数设成0，中断触发一次也会出窄脉冲。
    if(duty > 100) duty = 100;
    pwm_duty = duty;
    
    // ? 新增：0%占空比，直接关定时器+拉低IO
    if(duty == 0) {
        timer0_pwm_enable(0);
        return;
    }
    // ? 新增：100%占空比，直接关定时器+拉高IO
    if(duty >= 100) {
        timer0_pwm_enable(0);
        stc8g_gpio_set_level(3, 3, 1); // 直接拉高，不用定时器翻转
        return;
    }
    
    // 正常占空比才重新计算参数
    timer0_pwm_setfreq(pwm_freq);
    timer0_pwm_enable(1);
}

/**
 * 使能/失能PWM
 */
void timer0_pwm_enable(bit en)
{
    pwm_en = en;
    if(en) {
        // 启动时默认输出低电平，加载低电平初值
        pwm_current_level = 0;
        P33=0;
        TH0 = pwm_low_th;
        TL0 = pwm_low_tl;
        TR0 = 1; // 启动定时器0
    } else {
        TR0 = 0; // 停止定时器0
        P33=0; // 输出低电平
    }
}

/**
 * 定时器0中断服务函数
 * 核心逻辑：每次溢出翻转IO，重装对应电平的初值
 * 中断里只做最简操作，不耗时，符合你之前学的“中断快进快出”原则
 */
void Timer0_ISR(void) interrupt 1
{
    if(pwm_en == 0) return; // PWM未使能，直接退出

    if(pwm_current_level == 0) {
        // 当前是低电平，翻转为高电平，重装高电平初值
        TH0 = pwm_high_th;
        TL0 = pwm_high_tl;
		pwm_current_level = 1;
        P55=1;
    } else {
        // 当前是高电平，翻转为低电平，重装低电平初值
        TH0 = pwm_low_th;
        TL0 = pwm_low_tl;
		pwm_current_level = 0;
        P55=0;
    }
}