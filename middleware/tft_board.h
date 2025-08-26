#pragma once

#include <stdint.h>
#include "tft.h"


// Реализация функций для конкретной платы
void tft_set_dc(int state);
void tft_set_res(int state);
void tft_set_blk(int state);
void tft_delay_ms(uint32_t ms);
void tft_set_brightness(uint8_t value);



void tft_init_board_interface();