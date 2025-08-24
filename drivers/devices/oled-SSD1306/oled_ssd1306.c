#include "oled_ssd1306.h"
#include <stdio.h>

static OLED_Interface_t *oled_if = NULL;
static uint8_t oled_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];


void oled_send_cmd(uint8_t cmd);
void oled_send_data(uint8_t *data, uint32_t size);


void oled_send_cmd(uint8_t cmd)
{
    if (oled_if && oled_if->set_dc && oled_if->send)
    {
        oled_if->set_dc(0);
        oled_if->send(&cmd, 1);
    }
}

void oled_send_data(uint8_t *data, uint32_t size)
{
    if (oled_if && oled_if->set_dc && oled_if->send)
    {
        oled_if->set_dc(1);
        oled_if->send(data, size);
    }
}

void oled_clear()
{
    memset(oled_buffer, 0x00, sizeof(oled_buffer));
}

void oled_update_display()
{
    if (!oled_if)
        return;

    oled_send_cmd(SSD1306_CMD_SET_COL_ADDR);
    oled_send_cmd(0);
    oled_send_cmd(SSD1306_WIDTH - 1);

    oled_send_cmd(SSD1306_CMD_SET_PAGE_ADDR);
    oled_send_cmd(0);
    oled_send_cmd((SSD1306_HEIGHT / 8) - 1);

    oled_send_data(oled_buffer, sizeof(oled_buffer));
}

void oled_draw_pixel(uint8_t x, uint8_t y, OLED_Color color)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
        return;
    if (color)
        oled_buffer[x + (y / 8) * SSD1306_WIDTH] |= (1 << (y % 8));
    else
        oled_buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
}

void oled_set_background(OLED_Color color)
{
    if (color)
    {
        memset(oled_buffer, 0xFF, sizeof(oled_buffer));
    }
    else
    {
        memset(oled_buffer, 0x00, sizeof(oled_buffer));
    }
    oled_update_display(); // сразу отобразить
}

void oled_init(OLED_Interface_t *oled)
{
    oled_if = oled;
    if (!oled_if)
        return;

    // Сброс дисплея
    if (oled_if->set_res)
    {
        oled_if->set_res(0);
        oled_if->delay_ms(50);
        oled_if->set_res(1);
    }
    oled_if->delay_ms(50);

    // Последовательность инициализации SSD1306
    oled_send_cmd(SSD1306_CMD_DISPLAY_OFF); // 0xAE
    oled_send_cmd(SSD1306_CMD_SET_CLK_DIV); // 0xD5
    oled_send_cmd(0x80);                    // рекомендованное значение

    oled_send_cmd(SSD1306_CMD_SET_DISPLAY_OFFSET); // 0xD3
    oled_send_cmd(0x00);                           // без смещения

    oled_send_cmd(SSD1306_CMD_SET_START_LINE | 0x00); // 0x40

    oled_send_cmd(SSD1306_CMD_CHARGE_PUMP); // 0x8D
    oled_send_cmd(0x14);                    // включить зарядный насос

    oled_send_cmd(SSD1306_CMD_SET_MEMORY_MODE); // 0x20
    oled_send_cmd(0x00);                        // горизонтальная адресация

    oled_send_cmd(SSD1306_CMD_SET_SEG_REMAP | 0x01); // 0xA1 (зеркалим сегменты)

    oled_send_cmd(SSD1306_CMD_SET_COM_SCAN_DEC); // 0xC8 (обычно для 128x64)

    oled_send_cmd(0xDA); // Set COM pins config
    oled_send_cmd(0x12);

    oled_send_cmd(SSD1306_CMD_SET_CONTRAST); // 0x81
    oled_send_cmd(0x7F);                     // средний контраст

    oled_send_cmd(SSD1306_CMD_SET_PRECHARGE); // 0xD9
    oled_send_cmd(0xF1);

    oled_send_cmd(SSD1306_CMD_SET_VCOM_DETECT); // 0xDB
    oled_send_cmd(0x40);

    oled_send_cmd(SSD1306_CMD_DISPLAY_ALL_OFF); // 0xA4 (нормальный режим)
    oled_send_cmd(SSD1306_CMD_NORMAL_DISPLAY);  // 0xA6 (не инверсный)

    oled_send_cmd(SSD1306_CMD_DISPLAY_ON); // 0xAF (включить дисплей)
}
