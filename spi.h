#include "configuration.h"

#pragma once

#define SPI1_Reg 0x40013000U
#define SPI2_Reg 0x40003800U
#define SPI3_Reg 0x40003C00U

#define SPI1_NSS_Port 4  // PA4
#define SPI1_SCK_Port 5  // PA5
#define SPI1_MISO_Port 6 // PA6
#define SPI1_MOSI_Port 7 // PA7

#define SPI2_NSS_Port 12
#define SPI2_SCK_Port 13
#define SPI2_MISO_Port 14
#define SPI2_MOSI_Port 15

#define SPI3_NSS_Port SPI2_NSS_Port // PA15
#define SPI3_SCK_Port 3             // PB3
#define SPI3_MISO_Port 4            // PB4
#define SPI3_MOSI_Port 5            // PB5

typedef struct
{
    uint32_t CR1;
    uint32_t CR2;
    uint32_t SR;
    uint32_t DR;
    uint32_t CRCPR;
    uint32_t RXCRCR;
    uint32_t TXCRCR;
    uint32_t I2SCFGR;
    uint32_t I2SPR;
} SPI_Type;

#define SPI_CR1_DFF_16Bit (0b1 << 11)
#define SPI_CR1_SSM (0b1 << 9)
#define SPI_CR1_SSI (0b1 << 8)
#define SPI_CR1_SPE (0b1 << 6)


#define SPI_CR1_BIDMODE (0x1 << 15) 

#define SPI_CR1_BIDOE (0x1 << 14) 


// #define SPI_CR1_BR (0b100 << 3)
#define SPI_CR1_BR_16 (0b011 << 3)

#define SPI_CR1_MSTR (0b1 << 2)
#define SPI_CR1_CPOL (0b1 << 1)
#define SPI_CR1_CPHA (0b1 << 0)
// CR2
#define SPI_CR2_TXEIE (0b1 << 7)
#define SPI_CR2_RXNEIE (0b1 << 6)
#define SPI_CR2_ERRIE (0b1 << 5)
#define SPI_CR2_SSOE (0b1 << 2)
#define SPI_CR2_TXDMAEN (0b1 << 1)
#define SPI_CR2_RXDMAEN (0b1 << 0)

// SR
#define SPI_SR_BSY (0b1 << 7)
#define SPI_SR_OVR (0b1 << 6)
#define SPI_SR_MODF (0b1 << 5)
#define SPI_SR_CRCERR (0b1 << 4)
#define SPI_SR_UDR (0b1 << 3)
#define SPI_SR_CHSIDE (0b1 << 2)
#define SPI_SR_TXE (0b1 << 1)
#define SPI_SR_RXNE (0b1 << 0)

#define SPI_MASK_ERROR (SPI_SR_OVR | SPI_SR_CRCERR)

extern SPI_Type *spi_master;

int init_spi_master();

void send_data_spi(uint8_t *data, uint32_t size);
uint16_t recv_data_spi(SPI_Type *spi);

void send_data_spi_dma(uint8_t *message, uint16_t length);
void recive_data_spi_dma(uint8_t *message, uint16_t length);

void SPI1_IRQHandler();
