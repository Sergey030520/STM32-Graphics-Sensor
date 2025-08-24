#pragma once

// font5x7.h
#pragma once
#include <stdint.h>

typedef struct {
    const uint8_t *bitmap;  
    uint8_t char_width;     
    uint8_t char_height;    
    uint8_t char_count;      // количество символов в массиве
} GFX_Font_t;



extern const uint8_t font5x7[96][5];  

// структура шрифта
extern GFX_Font_t font5x7_struct;