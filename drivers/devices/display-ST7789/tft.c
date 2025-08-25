#include "tft.h"
#include <stdio.h>
#include "log.h"

// Res port - 0 - reset, 1 - start (PA1)
// DC - Data command (1 - data, 0 - command) (PA2)
// BLK - 1 - light, 0 - without light (PB6)

TFT_Interface_t *tft_if = NULL;

void send_command(uint8_t command);
void send_data_array_command(uint8_t *data, uint32_t size);
void send_data_command(uint8_t data);
void set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

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
        mode |= MADCLT_MX;
        break;
    case 2:
        mode |= MADCLT_MY;
        break;
    case 3:
        mode |= MADCLT_MX | MADCLT_MY;
        break;
    case 4:
        mode |= MADCLT_MV;
        break;
    case 5:
        mode |= MADCLT_MX | MADCLT_MV;
        break;
    case 6:
        mode |= MADCLT_ML | MADCLT_MH; // если оба зеркала
        break;
    case 7:
        mode |= MADCLT_MX | MADCLT_MY | MADCLT_MV;
        break;
    default:
        break;
    }

    if (vert_mirror)
        mode |= MADCLT_ML;
    if (horiz_mirror)
        mode |= MADCLT_MH;

    if (!isRgb)
        mode |= MADCLT_BGR;

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
    uint8_t params[] = {
        (startX >> 8), (startX & 0xFF),
        (endX >> 8), (endX & 0xFF)};
    send_data_array_command(params, sizeof(params));
}

void set_row_display(uint16_t startY, uint16_t endY)
{
    send_command(CMD_RASET);
    uint8_t params[] = {
        (startY >> 8), (startY & 0xFF),
        (endY >> 8), (endY & 0xFF)};
    send_data_array_command(params, sizeof(params));
}

void set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    set_column_display(x0, x1);
    set_row_display(y0, y1);
    send_command(CMD_RAMWR);
}

void draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT)
        return;

    set_address_window(x, y, x, y);

    uint8_t data[2] = {color >> 8, color & 0xFF};
    send_data_array_command(data, 2);
}

void st7789_fill_background(uint16_t color)
{
    if (!tft_if)
        return;

    set_address_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
    send_command(CMD_RAMWR);

    uint16_t buf[128];
    uint16_t swapped = (color >> 8) | (color << 8); // поменяли порядок байт
    for (int i = 0; i < 128; i++)
        buf[i] = swapped;

    uint32_t pixels = (uint32_t)DISPLAY_WIDTH * DISPLAY_HEIGHT;
    while (pixels > 0)
    {
        uint32_t batch = (pixels > 128) ? 128 : pixels;
        send_data_array_command((uint8_t *)buf, batch * 2);
        pixels -= batch;
    }
}

void st7789_init(TFT_Interface_t *tft_interface)
{
    if (tft_interface == NULL)
    {
        return;
    }
    tft_if = tft_interface;

    // 1. Hard Reset
    hard_reset();
    LOG_INFO("1. Hard Reset done\r\n");

    // 2. Software Reset
    soft_reset();
    tft_if->delay_ms(150);
    LOG_INFO("2. Software Reset done\r\n");

    // 3. Sleep Out
    sleep_mode_display(0); // SLPOUT
    tft_if->delay_ms(120);
    LOG_INFO("3. Sleep Out\r\n");

    // 4. Color Mode (16 bit/pixel)
    set_color_mode(BIT_PIXEl_16 | COLOR_MODE_65K); // COLMOD
    tft_if->delay_ms(10);
    LOG_INFO("4. Color Mode set\r\n");

    // 5. Memory Access Control (MADCTL)
    set_MemAccessMode(0, 0, 0, 1);
    LOG_INFO("5. MADCTL set\r\n");

    // 6. Inversion ON (обычно нужно для ST7789)
    inversion_mode(1); // INVON
    LOG_INFO("6. Display inversion ON\r\n");

    // 7. Normal Display Mode
    send_command(CMD_NORON);
    tft_if->delay_ms(10);
    LOG_INFO("7. Normal Display Mode ON\r\n");

    // 8. Turn Display ON
    set_display_power(1); // DISPON
    tft_if->delay_ms(100);
    LOG_INFO("8. Display ON\r\n");

    // 9. Clear screen (чёрный фон)
    fill_color_display(0x0000);

    // 10. Подсветка
    if (tft_if->set_brightness)
        tft_if->set_brightness(100);
    LOG_INFO("9. Backlight ON\r\n");
}

void st7789_set_brightness(uint8_t volume)
{
    tft_if->set_brightness(volume);
}