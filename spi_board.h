#pragma once

#include <stdint.h>
#include "spi.h"


extern SPI_Config_t spi_cfg;


int init_spi();
int tft_spi_recv(uint8_t *data, uint32_t size);
int tft_spi_send(uint8_t *data, uint32_t size);