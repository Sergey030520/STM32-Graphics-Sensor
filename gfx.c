#include "gfx.h"
#include "graphics.h"
#include <stdio.h>


#ifdef TFT_DISPLAY
#include "tft_board.h"

#else
#include "oled_board.h"
#endif

static GFX_Display_t disp;
static GFX_Font_t *current_font = NULL;

static GFX_Color current_color = {.value = 1};
static GFX_Color current_bg_color = {.value = 0}; // по умолчанию черный

void gfx_drv_init()
{
#ifdef TFT_DISPLAY

#else
    oled_init_board_interface(); // инициализация дисплея

    disp.width = SSD1306_WIDTH;
    disp.height = SSD1306_HEIGHT;
    disp.draw_pixel = oled_draw_pixel;

#endif

    // установить шрифт по умолчанию
    current_font = &font5x7_struct;
}

void gfx_drv_set_color(GFX_Color color)
{
    current_color = color;
}

void gfx_drv_set_font(GFX_Font_t *font)
{
    current_font = font;
}

void gfx_drv_draw_string(uint16_t x, uint16_t y, const char *str)
{
    if (!current_font)
        return;
    gfx_draw_string(&disp, x, y, str, current_color);
}

void gfx_drv_clear()
{
#ifdef TFT_DISPLAY

#else
    oled_set_background(OLED_BLACK);
#endif
}

void gfx_drv_update()
{
#ifdef TFT_DISPLAY

#else
    oled_update_display();
#endif
}

void gfx_drv_set_background(GFX_Color color)
{
    current_bg_color = color;

#ifdef TFT_DISPLAY
    tft_fill_background(color); // если TFT поддерживает цвет
#else
    // Для OLED у нас только черный/белый
    if (color.value)
        oled_set_background(OLED_WHITE);
    else
        oled_set_background(OLED_BLACK);
#endif
}