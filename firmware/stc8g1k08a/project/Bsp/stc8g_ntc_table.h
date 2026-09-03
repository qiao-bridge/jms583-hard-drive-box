/*
 * stc8g_ntc_table.h
 * ==================
 * NTC 温度<->RAW 查表
 *
 * 方案：
 *   - 分压：VCC -> [NTC] -> ADC -> [R_FIX=10k] -> GND   (上NTC，下10k，已确认)
 *   - VREF = VCC，比值约掉，RAW = 1023 * 10k / (R_ntc + 10k)
 *   - 存储：一维 int16_t 数组，下标 i = 温度 + TEMP_OFFSET
 *           实际温度 = i - TEMP_OFFSET
 *   - 范围：-20 ~ 125 (1℃一格，共 146 项)，全 short，无 float
 */

#ifndef __STC8G_NTC_TABLE_H_
#define __STC8G_NTC_TABLE_H_

#include <STC8G.H>
#include <INTRINS.H>

/* ============ 用户配置区 ============ */

/* 下位固定电阻 (Ω)，按原理图修改 */
#define NTC_R_FIX       10000L

/* ADC 满量程 */
#define NTC_ADC_MAX     1023

/* 温度范围 */
#define NTC_T_MIN       (-20)
#define NTC_T_MAX       (125)
#define NTC_TABLE_SIZE  (NTC_T_MAX - NTC_T_MIN + 1)   /* = 146 */

/* 下标偏移：实际温度 = 下标 - TEMP_OFFSET */
#define TEMP_OFFSET     (20)

/* ============ 外部表声明 ============ */

/*
 * raw_table[i]：温度 (i - TEMP_OFFSET) 对应的 ADC 采集值 RAW
 * 范围 i=0..145，对应温度 -20..125
 */
extern const short raw_table[NTC_TABLE_SIZE];

/* ============ 函数声明 ============ */

/*
 * @brief  RAW(ADC值) -> 温度(℃)，二分查找 + 整数线性插值
 * @param  raw  10位 ADC 值 (0 ~ 1023)
 * @return 温度值(℃)，整数。越界时钳制到 -20 或 125
 */
int ntc_raw_to_temp(int raw);

#endif /* __STC8G_NTC_TABLE_H_ */
