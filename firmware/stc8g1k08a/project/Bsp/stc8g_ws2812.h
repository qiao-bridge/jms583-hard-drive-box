#ifndef __STC8G_WS2812_H_
#define __STC8G_WS2812_H_

#include <STC8G.H>
#include <INTRINS.H>

// ==================== 配置 ====================
#define WS2812_NUM      5       // 灯珠数量
#define WS2812_PORT     3       // 数据线端口
#define WS2812_PIN      1       // 数据线引脚 (P3.1)

// 颜色结构体
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGB_Color;

// ==================== 函数声明 ====================
void ws2812_init(void);
void ws2812_send_color(unsigned char index, RGB_Color color);
void ws2812_show_all(RGB_Color *colors);
void ws2812_clear(void);

// 温度灯效接口（主循环调用）
void ws2812_update_by_temp(int avg_temp);

#endif