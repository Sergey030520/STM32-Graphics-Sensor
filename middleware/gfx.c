#include "gfx.h"
#include "graphics.h"
#include <stdio.h>
#include "timer.h"
#include "log.h"

#ifdef TFT_DISPLAY
#include "tft_board.h"

#else
#include "oled_board.h"
#endif

static GFX_Display_t disp;
static GFX_Font_t *current_font = NULL;

#ifdef TFT_DISPLAY
static GFX_Color current_color = {.value = RGB565(255, 255, 255)};
static GFX_Color current_bg_color = {.value = RGB565(0, 0, 0)}; // по умолчанию черный
#else
static GFX_Color current_color = {.value = 1};
static GFX_Color current_bg_color = {.value = 0}; // по умолчанию черный
#endif



void gfx_draw_pixel_gfx(uint16_t x, uint16_t y, GFX_Color color)
{
#ifdef TFT_DISPLAY
      tft_draw_pixel(x, y, color);
#else
    OLED_Color oled_color = (color.value != 0 ? OLED_WHITE : OLED_BLACK);
    oled_draw_pixel((uint8_t)x, (uint8_t)y, oled_color);
#endif
}

void gfx_drv_init()
{
#ifdef TFT_DISPLAY
    tft_init_board_interface();
    fill_color_display(RGB565(0, 255, 0));
#else
    oled_init_board_interface(); // инициализация дисплея

    disp.width = SSD1306_WIDTH;
    disp.height = SSD1306_HEIGHT;
    disp.draw_pixel = gfx_draw_pixel_gfx;

#endif

    // установить шрифт по умолчанию
    current_font = &font5x7_struct;

    gfx_set_font(current_font);
    gfx_drv_clear();
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
    gfx_draw_string(&disp, x, y, str, current_color, current_bg_color);
}

void gfx_drv_draw_line(Point p0, Point p1)
{
    gfx_draw_line(&disp, p0, p1, current_color);
}

void gfx_drv_draw_rect(Point topLeft, Point bottomRight)
{
    gfx_draw_rect(&disp, topLeft, bottomRight, current_color);
}

void gfx_drv_draw_triangle(Point p0, Point p1, Point p2)
{
    gfx_draw_triangle(&disp, p0, p1, p2, current_color);
}

void gfx_drv_draw_circle(Point center, uint16_t radius)
{
    gfx_draw_circle(&disp, center, radius, current_color);
}

void gfx_drv_fill(Point start, Point end)
{
    gfx_fill(&disp, start, end, current_color);
}

void gfx_drv_clear()
{
    gfx_drv_set_background(current_bg_color);
}

void gfx_drv_update()
{
#ifndef TFT_DISPLAY
    oled_update_display();
#endif
}

void gfx_drv_set_background(GFX_Color color)
{
    current_bg_color = color;

#ifdef TFT_DISPLAY
    st7789_fill_background(color); // если TFT поддерживает цвет
#else
    if (color.value)
        oled_set_background(OLED_WHITE);
    else
        oled_set_background(OLED_BLACK);
#endif
}


void gfx_test_display()
{
    
    gfx_drv_fill((Point){0, 0}, (Point){disp.width - 1, disp.height - 1});

    
    gfx_drv_draw_line((Point){0, 0}, (Point){disp.width - 1, disp.height - 1});
    gfx_drv_draw_line((Point){0, disp.height - 1}, (Point){disp.width - 1, 0});

    
    gfx_drv_draw_rect((Point){10, 10}, (Point){50, 30});

    
    gfx_drv_draw_triangle((Point){60, 20}, (Point){90, 50}, (Point){40, 50});

    
    gfx_drv_draw_circle((Point){disp.width/2, disp.height/2}, 20);

    
    gfx_drv_draw_string(0, 0, "Test");

   
    gfx_drv_fill((Point){5, 5}, (Point){15, 15});
}
