#include "graphics.h"

#include "fonts.h"

static GFX_Font_t current_font = {0};

void gfx_set_font(GFX_Font_t *font)
{
    if (font)
    {
        current_font = *font;
    }
}

void gfx_draw_char(GFX_Display_t *disp, uint16_t x, uint16_t y, char c, GFX_Color fg, GFX_Color bg)
{
    if (!current_font.bitmap || current_font.char_count == 0)
    {
        return;
    }
    uint8_t index = (uint8_t)c - 32; // ASCII 32-127
    if (index >= current_font.char_count)
    {
        return;
    }

    const uint8_t *bitmap = &current_font.bitmap[index * current_font.char_width];

    for (uint8_t col = 0; col < current_font.char_width; col++)
    {
        uint8_t column_data = bitmap[col];
        for (uint8_t row = 0; row < current_font.char_height; row++)
        {
            uint8_t pixel = (column_data >> (current_font.char_height - 1 - row)) & 1;
            uint16_t px = x + col;
            uint16_t py = y + row;

            if (px < disp->width && py < disp->height)
            {
                disp->draw_pixel(px, py, pixel ? fg : bg);
            }
        }
    }
}

void gfx_draw_string(GFX_Display_t *disp, uint16_t x, uint16_t y, const char *str, GFX_Color fg, GFX_Color bg)
{
    if (!str || !current_font.bitmap || current_font.char_count == 0)
        return;

    uint16_t cursor_x = x;
    uint16_t cursor_y = y;

    for (; *str; str++)
    {
        char c = *str;

        if (c == '\n')
        {
            cursor_x = x;
            cursor_y += current_font.char_height + 1;

            continue;
        }

        uint8_t index = (uint8_t)c - 32;
        if (index >= current_font.char_count)
        {
            continue;
        }

        if (cursor_x + current_font.char_width > disp->width)
        {
            cursor_x = x;
            cursor_y += current_font.char_height + 1;
        }

        if (cursor_y + current_font.char_height > disp->height)
        {
            break;
        }

        gfx_draw_char(disp, cursor_x, cursor_y, c, fg, bg);
        cursor_x += current_font.char_width + 1;
    }
}

void gfx_draw_line(GFX_Display_t *disp, Point p0, Point p1, GFX_Color color)
{
    if (!disp || !disp->draw_pixel)
        return;

    int16_t dx = abs(p1.x - p0.x);
    int16_t sx = p0.x < p1.x ? 1 : -1;
    int16_t dy = -abs(p1.y - p0.y);
    int16_t sy = p0.y < p1.y ? 1 : -1;
    int16_t err = dx + dy;

    Point current = p0;

    while (1)
    {
        if (current.x >= 0 && current.x < disp->width &&
            current.y >= 0 && current.y < disp->height)
        {
            disp->draw_pixel(current.x, current.y, color);
        }

        if (current.x == p1.x && current.y == p1.y)
            break;

        int16_t e2 = 2 * err;
        if (e2 >= dy)
        {
            if (current.x == p1.x)
                break; // защита
            err += dy;
            current.x += sx;
        }
        if (e2 <= dx)
        {
            if (current.y == p1.y)
                break; // защита
            err += dx;
            current.y += sy;
        }
    }
}

void gfx_draw_rect(GFX_Display_t *disp, Point topLeft, Point bottomRight, GFX_Color color)
{
    if (!disp || !disp->draw_pixel)
        return;

    uint16_t x_start = topLeft.x < 0 ? 0 : (topLeft.x >= disp->width ? disp->width - 1 : topLeft.x);
    uint16_t y_start = topLeft.y < 0 ? 0 : (topLeft.y >= disp->height ? disp->height - 1 : topLeft.y);
    uint16_t x_end = bottomRight.x < 0 ? 0 : (bottomRight.x >= disp->width ? disp->width - 1 : bottomRight.x);
    uint16_t y_end = bottomRight.y < 0 ? 0 : (bottomRight.y >= disp->height ? disp->height - 1 : bottomRight.y);

    for (uint16_t x = x_start; x <= x_end; ++x)
    {
        for (uint16_t y = y_start; y <= y_end; ++y)
        {
            disp->draw_pixel(x, y, color);
        }
    }
}

void gfx_draw_triangle(GFX_Display_t *disp, Point p0, Point p1, Point p2, GFX_Color color)
{
    if (!disp || !disp->draw_pixel)
        return;

    if ((p0.x == p1.x && p0.y == p1.y) ||
        (p0.x == p2.x && p0.y == p2.y) ||
        (p1.x == p2.x && p1.y == p2.y))
        return;

    int16_t minX = p0.x < p1.x ? (p0.x < p2.x ? p0.x : p2.x) : (p1.x < p2.x ? p1.x : p2.x);
    int16_t minY = p0.y < p1.y ? (p0.y < p2.y ? p0.y : p2.y) : (p1.y < p2.y ? p1.y : p2.y);
    int16_t maxX = p0.x > p1.x ? (p0.x > p2.x ? p0.x : p2.x) : (p1.x > p2.x ? p1.x : p2.x);
    int16_t maxY = p0.y > p1.y ? (p0.y > p2.y ? p0.y : p2.y) : (p1.y > p2.y ? p1.y : p2.y);
    
    int32_t lambda1,lambda2, lambda3; 

    if (minX < 0) minX = 0;
    if (minY < 0)
        minY = 0;
    if (maxX >= disp->width)
        maxX = disp->width - 1;
    if (maxY >= disp->height)
        maxY = disp->height - 1;

    int32_t denom = (p1.y - p2.y) * (p0.x - p2.x) + (p2.x - p1.x) * (p0.y - p2.y);
    if (denom == 0) return;

     for (int16_t y = minY; y <= maxY; y++)
    {
        for (int16_t x = minX; x <= maxX; x++)
        {
            lambda1 = ((p1.y - p2.y) * (x - p2.x) + (p2.x - p1.x) * (y - p2.y)) / denom;
            lambda2 = ((p2.y - p0.y) * (x - p2.x) + (p0.x - p2.x) * (y - p2.y)) / denom;
            lambda3 = 1.0f - lambda1 - lambda2;

            if ((lambda1 >= 0 && lambda1 <= denom) &&
                (lambda2 >= 0 && lambda2 <= denom) &&
                (lambda3 >= 0 && lambda3 <= denom))
            {
                disp->draw_pixel(x, y, color);
            }
        }
    }
}

void gfx_draw_line_bresenham(GFX_Display_t *disp, Point p0, Point p1, GFX_Color color)
{
    if (!disp || !disp->draw_pixel)
        return;

    int16_t x = p0.x;
    int16_t y = p0.y;
    int16_t xS = (p0.x < p1.x) ? 1 : -1;
    int16_t yS = (p0.y < p1.y) ? 1 : -1;

    int16_t dx = abs(p1.x - x);
    int16_t dy = -abs(p1.y - y);
    int error = dx + dy;

    while (1)
    {
        if (x >= 0 && x < disp->width && y >= 0 && y < disp->height)
            disp->draw_pixel(x, y, color);

        if (x == p1.x && y == p1.y)
            break;

        int16_t e2 = 2 * error;

        if (e2 >= dy)
        {
            if (x == p1.x)
                break;
            error += dy;
            x += xS;
        }
        if (e2 <= dx)
        {
            if (y == p1.y)
                break;
            error += dx;
            y += yS;
        }
    }
}

void gfx_draw_circle(GFX_Display_t *disp, Point center, uint16_t radius, GFX_Color color)
{
    if (!disp || !disp->draw_pixel)
        return;

    int16_t x = 0;
    int16_t y = radius;
    int16_t d = 5 / 4 - radius;

    while (y >= x)
    {
        gfx_draw_line_bresenham(disp, (Point){center.x + x, center.y + y}, (Point){center.x + y, center.y + x}, color);
        gfx_draw_line_bresenham(disp, (Point){center.x + x, center.y - y}, (Point){center.x + y, center.y - x}, color);
        gfx_draw_line_bresenham(disp, (Point){center.x - x, center.y + y}, (Point){center.x - y, center.y + x}, color);
        gfx_draw_line_bresenham(disp, (Point){center.x - x, center.y - y}, (Point){center.x - y, center.y - x}, color);

        if (d > 0)
        {
            y--;
            d += 2 * (x - y) + 1;
        }
        else
        {
            d += 2 * x + 1;
        }
        x++;
    }
}

// void gfx_draw_image(GFX_Display_t *disp, uint16_t width, uint16_t height, GFX_Color pixels[DISPLAY_HEIGHT][DISPLAY_WIDTH])
// {
//     if (!disp || !disp->draw_pixel) return;

//     for (uint16_t y = 0; y < height && y < disp->height; ++y)
//     {
//         for (uint16_t x = 0; x < width && x < disp->width; ++x)
//         {
//             disp->draw_pixel(x, y, pixels[y][x]);
//         }
//     }
// }

void gfx_fill(GFX_Display_t *disp, Point start, Point end, GFX_Color color)
{
    if (!disp || !disp->draw_pixel)
        return;

    uint16_t x_start = (start.x < end.x) ? start.x : end.x;
    uint16_t x_end = (start.x > end.x) ? start.x : end.x;
    uint16_t y_start = (start.y < end.y) ? start.y : end.y;
    uint16_t y_end = (start.y > end.y) ? start.y : end.y;

    if (x_start >= disp->width)
        x_start = disp->width - 1;
    if (x_end >= disp->width)
        x_end = disp->width - 1;
    if (y_start >= disp->height)
        y_start = disp->height - 1;
    if (y_end >= disp->height)
        y_end = disp->height - 1;

    for (uint16_t y = y_start; y <= y_end; ++y)
    {
        for (uint16_t x = x_start; x <= x_end; ++x)
        {
            disp->draw_pixel(x, y, color);
        }
    }
}
