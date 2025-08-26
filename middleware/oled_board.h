#pragma once

#include <stdint.h>
#include "oled_ssd1306.h"


// Реализация функций для конкретной платы
void oled_set_dc(int state);
void oled_set_res(int state);
void oled_set_blk(int state);
void oled_delay_ms(uint32_t ms);
void oled_set_brightness(uint8_t value);

void oled_init_board_interface();