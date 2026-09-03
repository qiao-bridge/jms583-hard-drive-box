#include "stc8g_ws2812.h"
#include "stc8g_gpio.h"
#include "stc8g_delay.h"

// ==================== 内部变量 ====================
static RGB_Color current_colors[WS2812_NUM];
static unsigned int pos_acc = 0;          // 位置累加器（0~999）
static unsigned char current_index = 0;   // 当前亮灯索引

// ==================== 直接端口操作 ====================
#define WS2812_HIGH()  P3 |= 0x02
#define WS2812_LOW()   P3 &= 0xFD

// 11.0592MHz / 1T：1个_nop_() = 90.4ns
#define T1H()  do{ WS2812_HIGH(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); }while(0)
#define T1L()  do{ WS2812_LOW();  _nop_(); _nop_(); _nop_(); _nop_(); }while(0)
#define T0H()  do{ WS2812_HIGH(); _nop_(); _nop_(); _nop_(); }while(0)
#define T0L()  do{ WS2812_LOW();  _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); }while(0)

// ==================== 底层：发送一个字节 ====================
static void ws2812_send_byte(unsigned char dat)
{
    unsigned char mask;
    for (mask = 0x80; mask != 0; mask >>= 1) {
        if (dat & mask) { T1H(); T1L(); }
        else            { T0H(); T0L(); }
    }
}

// ==================== 对外接口 ====================
void ws2812_init(void)
{
    stc8g_gpio_init(3, 1, 1);
    WS2812_LOW();
    ws2812_clear();
}

void ws2812_send_color(unsigned char index, RGB_Color color)
{
    if (index < WS2812_NUM) {
        current_colors[index] = color;
    }
}

void ws2812_show_all(RGB_Color *colors)
{
    unsigned char i;
    EA = 0;
    for (i = 0; i < WS2812_NUM; i++) {
        ws2812_send_byte(colors[i].g);
        ws2812_send_byte(colors[i].r);
        ws2812_send_byte(colors[i].b);
    }
    EA = 1;
    WS2812_LOW();
    delay_us(60);
}

void ws2812_clear(void)
{
    unsigned char i;
    RGB_Color black = {0, 0, 0};
    for (i = 0; i < WS2812_NUM; i++) {
        current_colors[i] = black;
    }
    ws2812_show_all(current_colors);
}

// ==================== 色环查表：温度 → 颜色 ====================
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGB888;

static const RGB888 color_table[13] = {
    {  0,   0, 255},   // 20℃ 蓝
    {  0, 128, 255},   // 25℃
    {  0, 255, 255},   // 30℃ 青
    {  0, 255, 128},   // 35℃
    {  0, 255,   0},   // 40℃ 绿
    {128, 255,   0},   // 45℃
    {255, 255,   0},   // 50℃ 黄
    {255, 200,   0},   // 55℃
    {255, 128,   0},   // 60℃ 橙
    {255,  64,   0},   // 65℃
    {255,  32,   0},   // 70℃
    {255,  16,   0},   // 75℃
    {255,   0,   0},   // 80℃ 红
};

static unsigned char lerp(unsigned char a, unsigned char b, unsigned char frac)
{
    return a + ((unsigned int)(b - a) * frac / 32);
}

static RGB_Color temp_to_color(int avg_temp)
{
    RGB_Color c;
    if (avg_temp <= 20) {
        c.r = 0;   c.g = 0;   c.b = 160;   // 蓝色，略亮
        return c;
    } else if (avg_temp >= 80) {
        c.r = 255; c.g = 0;   c.b = 0;
        return c;
    } else {
        unsigned char idx = (avg_temp - 20) / 5;
        unsigned char seg = (avg_temp - 20) % 5;
        unsigned char frac = seg * 32 / 5;
        c.r = lerp(color_table[idx].r, color_table[idx+1].r, frac);
        c.g = lerp(color_table[idx].g, color_table[idx+1].g, frac);
        c.b = lerp(color_table[idx].b, color_table[idx+1].b, frac);
        return c;
    }
}

// ==================== 主接口：流水灯 + 温度决定速度 ====================
void ws2812_update_by_temp(int avg_temp)
{
    unsigned char i;
    RGB_Color base_color;
    unsigned int step;          // 累加器步进（0~999，越大越快）
    RGB_Color black = {0,0,0};

    // 1. 获取基础颜色
    base_color = temp_to_color(avg_temp);

    // 2. 计算步进：温度越高，step 越大
    //    20℃ → step=2，80℃ → step=122，差距61倍
    //    你可以自行调整系数，例如改成 (avg_temp-20)*3 获得更大差距
    if (avg_temp <= 20) {
        step = 2;
    } else if (avg_temp >= 80) {
        step = 130;             // 高温最快
    } else {
        step = 2 + (avg_temp - 20) * 1;   // 20℃:2, 80℃:122
    }

    // 3. 累加器推进
    pos_acc += step;
    if (pos_acc >= 800) {       // 阈值800，每800步移动一格（可调）
        pos_acc -= 800;
        current_index++;
        if (current_index >= WS2812_NUM) {
            current_index = 0;
        }
    }

    // 4. 设置灯阵：只有 current_index 亮，其余灭
    for (i = 0; i < WS2812_NUM; i++) {
        if (i == current_index) {
            ws2812_send_color(i, base_color);
        } else {
            ws2812_send_color(i, black);
        }
    }

    // 5. 显示
    ws2812_show_all(current_colors);
}