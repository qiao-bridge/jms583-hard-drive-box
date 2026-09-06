#include "stc8g_adc.h"


// ==================== 静态变量 ====================
static int filtered_temps[3] = {0, 0, 0};  // 滤波后的温度（0.1℃）

// ==================== 函数实现 ====================

bit ADCInit(void)
{
    P_SW2 |= 0x80;
    ADCTIM = 0x3F;
    P_SW2 &= 0x7F;
    ADCCFG = 0x2F;          // 右对齐，时钟分频
    ADC_CONTR = 0x80;       // 开启 ADC 电源，不选通道
    _nop_(); _nop_(); _nop_(); _nop_();
    _nop_(); _nop_(); _nop_(); _nop_();
    return 0;
}

int ADC_ReadChannel(unsigned char channel)
{
    int res;
    ADC_CONTR = (ADC_CONTR & 0xF0) | (channel & 0x0F);
    ADC_CONTR |= 0x40;	// adc转换启动控制位， 硬件自动清零
    _nop_(); _nop_();
    while (!(ADC_CONTR & 0x20)); // 等待adc采集完成
    ADC_CONTR &= ~0x20; //手动关闭adc标志位
    res = (ADC_RES << 8) | ADC_RESL;
    return res;
}


int ADC_SampleTemperatures(int *out_temps)
{
    int adc_val[3];
    int temps[3];
    int i;
    int avg;

    // 1. 读取三路 ADC（每次读取前自动切换通道）
    adc_val[0] = ADC_ReadChannel(2);  // P3.2 - JMS583
    adc_val[1] = ADC_ReadChannel(3);  // P3.3 - DCDC
    adc_val[2] = ADC_ReadChannel(4);  // P5.4 - SSD

    // 2. ADC → 温度  加的系数代表实际芯片的温度要比测量的要高一些
    for (i = 0; i < 3; i++) {
        temps[i] = ntc_raw_to_temp(adc_val[i]);
		switch(i){
			case 0:
				temps[i] += 35;
				break;
			case 1:
				temps[i] += 0;
				break;
			default:
				temps[i] += 30;
		}
    }

    // 3. 一阶低通滤波：out = out*(256-K)/256 + new*K/256
    for (i = 0; i < 3; i++) {
		filtered_temps[i] = temps[i];
        filtered_temps[i] = (filtered_temps[i] * (256 - FILTER_K) + temps[i] * FILTER_K) >> 8;
        out_temps[i] = filtered_temps[i];
    }

    // 4. 加权平均（权重之和 = 100）
    avg = (filtered_temps[SENSOR_JMS583] * WEIGHT_JMS583 +
           filtered_temps[SENSOR_DCDC]   * WEIGHT_DCDC +
           filtered_temps[SENSOR_SSD]    * WEIGHT_SSD) / 100;

    return avg;
}

// 内部参考电压校准
static void ADCInitInside(void)
{
    P_SW2 |= 0x80;
    ADCTIM = 0x3F;
    P_SW2 &= 0x7F;
    ADCCFG = 0x2F;
    ADC_CONTR = 0x8F;  // 内部 1.19V 通道
    _nop_(); _nop_(); _nop_(); _nop_();
    _nop_(); _nop_(); _nop_(); _nop_();
}

int adcreference(void)
{
    unsigned char i;
    unsigned int res = 0;
    unsigned int adc_val = 0;
    ADCInitInside();
    ADC_ReadChannel(15);  // 丢弃第一次，通道15用于检测内部adc的参考电压
    for (i = 0; i < 8; i++) {
        res += ADC_ReadChannel(15);
    }
    adc_val = res / 8;
    return (1190UL * 1023UL) / adc_val;
}

void ADCPowerOff(void)
{
    ADC_CONTR &= ~0x80;
}
