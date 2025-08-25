#pragma once
#include <stdint.h>
#include "graphics.h"
#include "fonts.h"
#include "math_pixels.h"


typedef struct {
    uint32_t value;
} GFX_Color;


typedef struct {
    void (*draw_pixel)(uint16_t x, uint16_t y, GFX_Color color);
    uint16_t width;
    uint16_t height;
} GFX_Display_t;


void gfx_set_font(GFX_Font_t *font);
void gfx_draw_char(GFX_Display_t *disp, uint16_t x, uint16_t y, char c, GFX_Color fg, GFX_Color bg);
void gfx_draw_string(GFX_Display_t *disp, uint16_t x, uint16_t y, const char *str, GFX_Color fg, GFX_Color bg);
void gfx_fill(GFX_Display_t *disp, Point start, Point end, GFX_Color color);
void gfx_draw_circle(GFX_Display_t *disp, Point center, uint16_t radius, GFX_Color color);
void gfx_draw_triangle(GFX_Display_t *disp, Point p0, Point p1, Point p2, GFX_Color color);
void gfx_draw_line(GFX_Display_t *disp, Point p0, Point p1, GFX_Color color);
void gfx_draw_rect(GFX_Display_t *disp, Point topLeft, Point bottomRight, GFX_Color color);
