#include <STC8G.H>
#include "stc8g_gpio.h"
#include "stc8g_uart.h"
#include "stc8g_delay.h"
#include "stc8g_adc.h"
#include "stc8g_timer_pwm.h"  
#include "stc8g_ws2812.h"

/* ================= 用户配置：温度-转速曲线 ================= */
#define TEMP_LOW     25    /* 低于 25.0℃：风扇停转 */
#define TEMP_HIGH    60    /* 高于 60.0℃：风扇满速 */
#define HYSTERESIS   2     /* 2.0℃ 滞回，防止临界点频繁启停 */


/* 三路权重（与 stc8g_adc.h 中 TEMP_WEIGHT 保持一致，仅用于打印说明） */

/**
 * @brief 根据平均温度计算并设置风扇 PWM
 * @param  avg_temp: 加权平均温度 (0.1℃)
 */
static void Fan_Update(int avg_temp)
{
    static unsigned char last_duty = 0;
    unsigned char target;

    if (avg_temp < TEMP_LOW) {
        target = 0;
    } else if (avg_temp >= TEMP_HIGH) {
        target = 100;
    } else {
        /* 线性：25~60℃ 对应 0%~100% */
        target = (unsigned char)((avg_temp - TEMP_LOW) * 100L / (TEMP_HIGH - TEMP_LOW));
    }

    /* 迟滞：只有变化超过HYSTERESIS时才更新 */
    if (target > last_duty + HYSTERESIS) {
        last_duty = target;
    } else if (target + HYSTERESIS < last_duty) {
        last_duty = target;
    }
    timer0_pwm_setduty(last_duty);   
}

void main(void)
{
    int temps[3];
    int avg_temp;

    // 1. GPIO 初始化
    stc8g_gpio_init(3, 0, 2);  // P3.0 RXD
    stc8g_gpio_init(3, 1, 1);  // P3.1 TXD
    stc8g_gpio_init(3, 2, 2);  // P3.2 ADC 输入
    stc8g_gpio_init(3, 3, 2);  // P3.3 ADC 输入
    stc8g_gpio_init(5, 4, 2);  // P5.4 ADC 输入

    // 2. 串口初始化
    //UartInit();

    // 3. ADC 初始化
	ADCInit();  // 仅一次

    // 4. 校准参考电压（可选）
    //int vref = adcreference();

    // 5. PWM 初始化
    timer0_pwm_init(5, 5);		// P5.5 PWM 输出
    timer0_pwm_setfreq(21600);
    timer0_pwm_setduty(50);
    timer0_pwm_enable(1);
	
	// 6.ws2812呼吸灯初始化
	ws2812_init();

    ES = 1;
    EA = 1;
	
	
    while(1)
    {
        // 采集温度并获取加权平均
        avg_temp = ADC_SampleTemperatures(temps);

        // 根据温度设置 PWM
        Fan_Update(avg_temp);
		
		// 更新 WS2812 灯效
		ws2812_update_by_temp(avg_temp);

        delay_ms(1);
    }
}
