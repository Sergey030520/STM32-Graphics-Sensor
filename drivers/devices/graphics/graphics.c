#include "graphics.h"

#include "fonts.h"

static GFX_Font_t current_font = {0};  


void gfx_set_font(GFX_Font_t *font)
{
    if(font) {
        current_font = *font;
    }
}


void gfx_draw_char(GFX_Display_t *disp, uint16_t x, uint16_t y, char c, GFX_Color color)
{
    if(current_font.char_count == 0 || !current_font.bitmap) return;

    uint8_t index = (uint8_t)c - 32; // если массив начинается с ASCII 32
    if(index >= current_font.char_count) return;

    const uint8_t *bitmap = &current_font.bitmap[index * current_font.char_width];

    for(uint8_t i = 0; i < current_font.char_width; i++)
    {
        uint8_t col = bitmap[i];
        for(uint8_t j = 0; j < current_font.char_height; j++)
        {
            if(col & (1 << j))
            {
                uint16_t px = x + i;
                uint16_t py = y + j;
                if(px < disp->width && py < disp->height)
                    disp->draw_pixel(px, py, color);
            }
        }
    }
}

void gfx_draw_string(GFX_Display_t *disp, uint16_t x, uint16_t y, const char *str, GFX_Color color)
{
    if (!str || !current_font.bitmap || current_font.char_count == 0) return;

    uint16_t cursor_x = x;
    uint16_t cursor_y = y;

    for (; *str; str++)
    {
        char c = *str;
        uint8_t index = (uint8_t)c - 32; // если массив начинается с ASCII 32
        if (index >= current_font.char_count)
            continue; // символа нет в шрифте

        // Проверка выхода за ширину дисплея
        if (cursor_x + current_font.char_width >= disp->width)
        {
            cursor_x = 0;
            cursor_y += current_font.char_height + 1; // перенос на следующую строку
            if (cursor_y + current_font.char_height >= disp->height)
                break; // место закончилось
        }

        // Рисуем символ
        gfx_draw_char(disp, cursor_x, cursor_y, c, color);

        cursor_x += current_font.char_width + 1; // шаг по X между символами
    }
}
