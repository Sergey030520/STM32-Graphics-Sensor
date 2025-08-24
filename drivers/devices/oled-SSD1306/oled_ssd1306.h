#pragma once

#include <stdint.h>

// Разрешение дисплея
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

#define SSD1306_I2C_ADDR 0x3C

// Макросы команд
#define SSD1306_CMD_DISPLAY_OFF 0xAE
#define SSD1306_CMD_DISPLAY_ON 0xAF
#define SSD1306_CMD_SET_CONTRAST 0x81
#define SSD1306_CMD_NORMAL_DISPLAY 0xA6
#define SSD1306_CMD_INVERT_DISPLAY 0xA7
#define SSD1306_CMD_DISPLAY_ALL_ON 0xA5
#define SSD1306_CMD_DISPLAY_ALL_OFF 0xA4
#define SSD1306_CMD_SET_MEMORY_MODE 0x20
#define SSD1306_CMD_SET_PAGE_ADDR 0x22
#define SSD1306_CMD_SET_COL_ADDR 0x21
#define SSD1306_CMD_SET_START_LINE 0x40
#define SSD1306_CMD_SET_SEG_REMAP 0xA0
#define SSD1306_CMD_SET_COM_SCAN_INC 0xC0
#define SSD1306_CMD_SET_COM_SCAN_DEC 0xC8
#define SSD1306_CMD_SET_DISPLAY_OFFSET 0xD3
#define SSD1306_CMD_SET_CLK_DIV 0xD5
#define SSD1306_CMD_SET_PRECHARGE 0xD9
#define SSD1306_CMD_SET_VCOM_DETECT 0xDB
#define SSD1306_CMD_CHARGE_PUMP 0x8D

typedef void (*pin_ctrl_t)(int state);
typedef int (*bus_transfer_t)(uint8_t *data, uint32_t size);
typedef void (*delay_timer_t)(uint32_t ms);
typedef void (*brightness_func_t)(uint8_t volume);

typedef struct
{
    pin_ctrl_t set_dc;
    pin_ctrl_t set_res;
    bus_transfer_t send;
    bus_transfer_t recv;
    delay_timer_t delay_ms;
} OLED_Interface_t;

typedef enum
{
    OLED_BLACK = 0,
    OLED_WHITE
} OLED_Color;

void oled_init(OLED_Interface_t *oled);
void oled_draw_pixel(uint8_t x, uint8_t y, OLED_Color color);
void oled_set_background(OLED_Color color);
void oled_update_display();
void oled_clear();