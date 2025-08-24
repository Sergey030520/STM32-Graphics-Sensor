#pragma once

#include <stdint.h>
#include "graphics.h" 

void gfx_drv_init();  
void gfx_drv_set_font(GFX_Font_t *font);
void gfx_drv_draw_string(uint16_t x, uint16_t y, const char *str);
void gfx_drv_clear();
void gfx_drv_update();
void gfx_drv_set_background(GFX_Color color);