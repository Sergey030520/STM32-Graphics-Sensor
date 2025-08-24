#include "tft.h"
#include <stdio.h>
#include "log.h"

// Res port - 0 - reset, 1 - start (PA1)
// DC - Data command (1 - data, 0 - command) (PA2)
// BLK - 1 - light, 0 - without light (PB6)

TFT_Interface_t *tft_if = NULL;

void send_command(uint8_t command)
{
    if (tft_if == NULL)
        return;
    tft_if->set_dc(0);
    tft_if->spi_send(&command, 1);
}
void send_data_array_command(uint8_t *data, uint32_t size)
{
    if (tft_if == NULL)
        return;
    tft_if->set_dc(1);
    tft_if->spi_send(data, size);
}

void send_data_command(uint8_t data)
{
    if (tft_if == NULL)
        return;
    tft_if->set_dc(1);
    tft_if->spi_send(&data, 1);
}

void hard_reset()
{
    if (tft_if == NULL)
        return;
    tft_if->set_res(0);
    tft_if->delay_ms(10);
    tft_if->set_res(1);
    tft_if->delay_ms(150);
}

void soft_reset()
{
    if (tft_if == NULL)
        return;
    send_command(CMD_SWRESET);
    tft_if->delay_ms(100);
}

void sleep_mode_display(int state)
{
    if (state)
    {
        send_command(CMD_SLPIN);
    }
    else
    {
        send_command(CMD_SLPOUT);
    }
    tft_if->delay_ms(200);
}

void inversion_mode(int state)
{
    if (state)
    {
        send_command(CMD_INVON);
    }
    else
    {
        send_command(CMD_INVOFF);
    }
}

void set_color_mode(uint8_t mode)
{
    send_command(CMD_COLMOD);
    send_data_command(mode);
}

void set_MemAccessMode(uint8_t rotation, uint8_t vert_mirror, uint8_t horiz_mirror, uint8_t isRgb)
{
    uint8_t mode = 0;
    send_command(CMD_MADCTL);

    switch (rotation)
    {
    case 1:
        mode = MADCLT_MX;
        break;
    case 2:
        mode = MADCLT_MY;
        break;
    case 3:
        mode = MADCLT_MX | MADCLT_MY;
        break;
    case 4:
        mode = MADCLT_MV;
        break;
    case 5:
        mode = MADCLT_MX | MADCLT_MV;
        break;
    case 6:
        if (vert_mirror)
            mode |= MADCLT_ML;
        if (horiz_mirror)
            mode |= MADCLT_MH;
        break;
    case 7:
        mode = MADCLT_MX | MADCLT_MY | MADCLT_MV;
        break;
    default:
        break;
    }
    if (vert_mirror)
        mode = MADCLT_ML;
    if (horiz_mirror)
        mode = MADCLT_MH;

    if (!isRgb)
        mode |= (0x1 << 3);

    send_data_command(mode);
}

void set_display_power(int state)
{
    if (state)
    {
        send_command(CMD_DISPON);
    }
    else
    {
        send_command(CMD_DISPOFF);
    }
}

void set_porch_settings()
{
    send_command(CMD_PORCTRL);
    uint8_t porch_settings[] = {0x0C, 0x0C, 0x0, 0x33, 0x33};
    send_data_array_command(porch_settings, sizeof(porch_settings));
}

void set_column_display(uint16_t startX, uint16_t endX)
{
    send_command(CMD_CASET);
    // uint8_t params[] = {(startX >> 8), (startX & 0xFF), (endX >> 8), (endX & 0xFF)};
    // send_data_array_command(params, sizeof(params));
    send_data_command(startX >> 8);
    send_data_command(startX & 0xFF);
    send_data_command(endX >> 8);
    send_data_command(endX & 0xFF);
}
void set_row_display(uint16_t startY, uint16_t endY)
{
    send_command(CMD_RASET);
    // uint8_t params[] = {startY >> 8, startY & 0xFF, endY >> 8, endY & 0xFF};
    // send_data_array_command(params, sizeof(params));
    send_data_command(startY >> 8);
    send_data_command(startY & 0xFF);
    send_data_command(endY >> 8);
    send_data_command(endY & 0xFF);
}

void set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    set_column_display(x0, x1);
    set_row_display(y0, y1);
    send_command(CMD_RAMWR);
}

void fill_color_display(uint16_t color)
{
    if (!tft_if)
        return;

    set_address_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
    send_command(CMD_RAMWR);

    uint16_t buf[128];
    for (int i = 0; i < 128; i++)
        buf[i] = (color >> 8) | (color << 8); // меняем порядок байт для SPI

    uint32_t pixels = DISPLAY_WIDTH * DISPLAY_HEIGHT;
    while (pixels > 0)
    {
        uint32_t toSend = pixels > 128 ? 128 : pixels;
        send_data_array_command((uint8_t *)buf, toSend * 2);
        pixels -= toSend;
    }
}

void fill_screen(uint16_t color)
{
    if (!tft_if)
        return;

    // задаём область на весь экран
    set_address_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);

    // готовим цвет в формате big-endian (старший байт первым)
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    // проходимся по всем пикселям и пишем по два байта
    for (uint32_t i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++)
    {
        send_data_command(hi);
        send_data_command(lo);
    }
}

void draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{

    uint16_t j = 0;

    for (uint16_t i = x0; i <= x1; ++i)
    {
        for (j = y0; j <= y1; ++j)
        {
            draw_pixel(i, j, color);
        }
    }
}

void draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    if ((x + width) > DISPLAY_WIDTH)
    {
        // ledOn(1);
        return;
    }
    if ((y + height) > DISPLAY_HEIGHT)
    {
        // signal_error(5);
        return;
    }

    uint8_t data_color[] = {color >> 8, color & 0xFF};

    // Coordinate coord = {x, y, (x + width) - 1, (y + height) - 1};

    set_address_window(x, y, (x + width) - 1, (y + height) - 1);

    uint16_t j = 0;
    for (uint16_t idx = 0; idx < width; ++idx)
    {
        for (j = 0; j < height; ++j)
        {
            send_data_command(color >> 8);
            send_data_command(color & 0xFF);
            // send_data_array_command(data_color, sizeof(data_color));
        }
    }
}

void draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT)
    {
        return;
    }
    if (y < 0 || y > DISPLAY_HEIGHT)
    {
        return;
    }

    set_address_window(x, y, x, y);
    send_data_command(color >> 8);
    send_data_command(color & 0xFF);
    // uint8_t data_color[] = {color >> 8, color & 0xFF};
    // send_data_array_command(data_color, sizeof(data_color));
}

void draw_triangle(Point point1, Point point2, Point point3, uint16_t color)
{
    Point points[] = {point1, point2, point3};
    Point startPoint = findMinPoint(points, 3),
          endPoint = findMaxPoint(points, 3);

    float bar_coord1 = 0, bar_coord2 = 0, bar_coord3 = 0;
    for (int16_t yV = startPoint.y; yV < endPoint.y; ++yV)
    {
        for (int16_t xH = startPoint.x; xH < endPoint.x; ++xH)
        {
            bar_coord1 = ((point2.y - point3.y) * (xH - point3.x) + (point3.x - point2.x) * (yV - point3.y)) / ((point2.y - point3.y) * (point1.x - point3.x) + (point3.x - point2.x) * (point1.y - point3.y));
            bar_coord2 = ((point3.y - point1.y) * (xH - point3.x) + (point1.x - point3.x) * (yV - point3.y)) / ((point2.y - point3.y) * (point1.x - point3.x) + (point3.x - point2.x) * (point1.y - point3.y));

            bar_coord3 = 1 - bar_coord1 - bar_coord2;
            if (bar_coord1 >= 0 && bar_coord1 <= 1 && bar_coord2 >= 0 && bar_coord2 <= 1 && bar_coord3 >= 0 && bar_coord3 <= 1)
            {
                draw_pixel(xH, yV, color);
            }
        }
    }
}

void draw_line_bresenham(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    uint16_t x = x0, y = y0;

    int16_t xS = 1, yS = 1;
    if (x0 > x1)
    {
        xS = -1;
    }
    if (y0 > y1)
    {
        yS = -1;
    }

    int16_t dy = (-1) * abs(y1 - y);
    int16_t dx = abs(x1 - x);
    int error = dx + dy;
    int error2 = 0;
    while (1)
    {
        draw_pixel(x, y, color);

        if (x == x1 && y == y1)
        {
            break;
        }
        error2 = error * 2;

        if (error2 >= dy)
        {
            if (x == x1)
            {
                break;
            }
            error += dy;
            x += xS;
        }
        if (error2 <= dx)
        {
            if (y == y1)
            {
                break;
            }
            error += dx;
            y += yS;
        }
    }
}

void draw_circle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color)
{
    uint16_t x = 0, y = radius;
    int d = 5 / 4 - radius;

    while (y > x)
    {
        draw_line_bresenham(x + x0, y + y0, y + y0, x + x0, color);
        draw_line_bresenham(x0 + x, y0 - y, y0 - y, x0 + x, color);
        draw_line_bresenham(y0 - x, x0 - y, x0 - y, y0 - x, color);
        draw_line_bresenham(x0 - x, y + y0, y + y0, x0 - x, color);
        if (d > 0)
        {
            y--;
            d += (2 * (x - y) + 1);
        }
        else
            d += (2 * x + 1);
        x++;
    }
}

void draw_image(uint16_t width, uint16_t heigth, uint16_t pixels[DISPLAY_HEIGHT][DISPLAY_WIDTH])
{

    set_address_window(0, 0, width, heigth);
    uint16_t row, column;
    for (row = 0; row < DISPLAY_HEIGHT; ++row)
    {
        for (column = 0; column < DISPLAY_WIDTH; ++column)
        {
            uint8_t data[] = {pixels[row][column] >> 8, pixels[row][column] && 0xFF};
            send_data_array_command(data, sizeof(data));
        }
    }
}

void fill(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY, uint16_t color)
{
    if (startX < 0 || startX >= DISPLAY_WIDTH || endX < 0 || endX >= DISPLAY_WIDTH)
    {
        return;
    }

    if (startY < 0 || startY >= DISPLAY_HEIGHT || endY < 0 || endY >= DISPLAY_HEIGHT)
    {
        return;
    }

    set_address_window(startX, startY, endX, endY);

    uint16_t column = 0;
    uint16_t row = 0;

    for (column = startX; column <= endX; ++column)
    {
        for (row = startY; row <= endY; ++row)
        {
            uint8_t data_color[] = {color >> 8, color & 0xFF};
            send_data_array_command(data_color, sizeof(data_color));
        }
    }
}

void clear_screen()
{
    fill_color_display(RGB565(0, 0, 0));
}

// void st7789_init(TFT_Interface_t *tft_interface)
// {
//     if (tft_interface == NULL)
//     {
//         return;
//     }
//     tft_if = tft_interface;

//     hard_reset();
//     LOG_INFO("1. set hard_reset\r\n");

//     soft_reset();
//     LOG_INFO("2. soft reset\r\n");

//     tft_if->delay_ms(150);

//     sleep_mode_display(0);

//     LOG_INFO("3. sleep mode display\r\n");

//     set_color_mode(BIT_PIXEl_16 | COLOR_MODE_65K);
//     set_MemAccessMode(4, 0, 0, 1);

//     LOG_INFO("4. set color\r\n");

//     tft_if->delay_ms(10);

//     inversion_mode(1);

//     LOG_INFO("5. inversion mode\r\n");

//     send_command(CMD_NORON);

//     LOG_INFO("6. send cmd noron\r\n");

//     fill_color_display(0x00);

//     LOG_INFO("7. fill color display\r\n");

//     set_display_power(1);

//     LOG_INFO("8. set display power\r\n");

//     tft_if->delay_ms(100);
//     tft_if->set_brightness(100);
// }

void st7789_init(TFT_Interface_t *tft_interface)
{
    tft_if = tft_interface;

    hard_reset();
    tft_if->delay_ms(120);

    send_command(CMD_SLPOUT); // 0x11 — выход из сна
    tft_if->delay_ms(120);

    send_command(CMD_COLMOD); // 0x3A — формат цвета
    send_data_command(0x55);  // 16 бит на пиксель (RGB565)
    tft_if->delay_ms(10);

    send_command(CMD_MADCTL); // 0x36 — ориентация и порядок цветов
    send_data_command(MADCLT_MX | MADCLT_MY | MADCLT_BGR);  // попробуем дефолт: RGB, без зеркал
    tft_if->delay_ms(10);

    // send_command(CMD_INVON); // 0x21 — включить инверсию (некоторые панели требуют)
    // tft_if->delay_ms(10);

    send_command(CMD_DISPON); // 0x29 — включить дисплей
    tft_if->delay_ms(100);

    // тестовая заливка
    fill_screen(RGB565(255, 0, 0));
}

void set_brightness(uint8_t volume){
    tft_if->set_brightness(volume);
}