#pragma once

#include <stdint.h>
#include "graphics.h" 

void gfx_drv_init();  
void gfx_drv_set_font(GFX_Font_t *font);
void gfx_drv_draw_string(uint16_t x, uint16_t y, const char *str);
void gfx_drv_clear();
void gfx_drv_update();
void gfx_drv_set_background(GFX_Color color);
void gfx_drv_draw_pixel(uint16_t x, uint16_t y);
void gfx_drv_draw_line(Point p0, Point p1);
void gfx_drv_draw_rect(Point topLeft, Point bottomRight);
void gfx_drv_draw_triangle(Point p0, Point p1, Point p2);
void gfx_drv_draw_circle(Point center, uint16_t radius);
void gfx_drv_fill(Point start, Point end);

void gfx_test_display();