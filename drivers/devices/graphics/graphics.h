#pragma once
#include <stdint.h>
#include "graphics.h"
#include "fonts.h"


typedef struct {
    uint32_t value;
} GFX_Color;


typedef struct {
    void (*draw_pixel)(uint16_t x, uint16_t y, GFX_Color color);
    uint16_t width;
    uint16_t height;
} GFX_Display_t;


void gfx_set_font(GFX_Font_t *font);
// Примитивы
// void gfx_draw_line(GFX_Display_t *disp, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, GFX_Color color);
// void gfx_draw_rect(GFX_Display_t *disp, uint16_t x, uint16_t y, uint16_t w, uint16_t h, GFX_Color color);
// void gfx_fill_rect(GFX_Display_t *disp, uint16_t x, uint16_t y, uint16_t w, uint16_t h, GFX_Color color);
// void gfx_draw_triangle(GFX_Display_t *disp, uint16_t x0, uint16_t y0,
                    //    uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, GFX_Color color);
void gfx_draw_char(GFX_Display_t *disp, uint16_t x, uint16_t y, char c, GFX_Color color);
void gfx_draw_string(GFX_Display_t *disp, uint16_t x, uint16_t y, const char *str, GFX_Color color);