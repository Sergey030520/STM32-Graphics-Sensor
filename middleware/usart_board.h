#pragma once

#include "usart.h"
#include <stdint.h>



void board_init_uart();
int usart_adapter(const char *data, int length);
