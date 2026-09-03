#ifndef __STC8G_ADC_H_
#define __STC8G_ADC_H_

#include <STC8G.H>
#include <INTRINS.H>
#include "stc8g_gpio.h"
#include "stc8g_delay.h"
#include "stc8g_uart.h"
#include "stc8g_ntc_table.h"

// ==================== 用户配置区 ====================

// NTC 参数（上NTC 100k + 下10k）
#define NTC_R0              100000UL    // 25℃ 时 NTC 阻值（单位 Ω）
#define NTC_BETA            3950        // NTC B 值（按实际修改）
#define NTC_VSUPPLY_MV      5000UL      // 分压供电电压（mV），这里是 5V

// 三路 ADC 对应的温度传感器编号（用于加权）
#define SENSOR_JMS583   0   // JMS583 芯片旁
#define SENSOR_DCDC     1   // DCDC 电源旁
#define SENSOR_SSD      2   // M.2 固态下方

// 温度权重（总和应为 1.0，这里用整数表示，分母 100）
#define WEIGHT_JMS583   30  // 30%
#define WEIGHT_DCDC     45  // 45%
#define WEIGHT_SSD      25  // 25%

// 软件一阶低通滤波系数（0~255，越大越平滑）
#define FILTER_K        32  // 新值占比 32/256 = 12.5%

// ===================================================

/**
 * @brief ADC 初始化（仅初始化模块，不固定通道）
 * @param adc_channel 忽略，保留参数仅为兼容
 * @return bit 0=成功
 */
bit ADCInit(void);

/**
 * @brief 读取指定通道的 ADC 值（自动切换通道）
 * @param channel 0~5（P3.0~P3.3, P5.4, P5.5）
 * @return 10位 ADC 值 (0~1023)
 */
int ADC_ReadChannel(unsigned char channel);

/**
 * @brief 采集三路温度并返回加权平均值（已包含滤波）
 * @param out_temps 输出数组，长度为3，存放三路温度（0.1℃）
 * @return 加权平均温度（0.1℃）
 */
int ADC_SampleTemperatures(int *out_temps);

/**
 * @brief 获取内部参考电压（mV）
 * @return 实际 Vref（mV）
 */
int adcreference(void);

/**
 * @brief 关闭 ADC 电源
 */
void ADCPowerOff(void);

#endif
