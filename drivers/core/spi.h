#pragma once

#include <stdint.h>
#include "dma.h"
#include "gpio.h"

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

// CR1
#define SPI_CR1_BIDIMODE (0x1 << 15)
#define SPI_CR1_BIDOE (0x1 << 14)
#define SPI_CR1_CRCEN (0x1 << 13)
#define SPI_CR1_CRCNEXT (0x1 << 12)
#define SPI_CR1_DFF (0x1 << 11)
#define SPI_CR1_RXONLY (0x1 << 10)
#define SPI_CR1_SSM (0x1 << 9)
#define SPI_CR1_SSI (0x1 << 8)
#define SPI_CR1_LSBFIRST (0x1 << 7)
#define SPI_CR1_SPE (0x1 << 6)
#define SPI_CR1_BR(baud_rate) ((baud_rate & 0x7) << 3)
#define SPI_CR1_MSTR (0x1 << 2) // 1 = master, 0 = slave
#define SPI_CR1_CPOL (0x1 << 1)
#define SPI_CR1_CPHA (0x1 << 0)

typedef enum
{
    SPI_BAUDRATE_DIV2 = 0x0,
    SPI_BAUDRATE_DIV4 = 0x1,
    SPI_BAUDRATE_DIV8 = 0x2,
    SPI_BAUDRATE_DIV16 = 0x3,
    SPI_BAUDRATE_DIV32 = 0x4,
    SPI_BAUDRATE_DIV64 = 0x5,
    SPI_BAUDRATE_DIV128 = 0x6,
    SPI_BAUDRATE_DIV256 = 0x7
} SPI_BaudRateControl;

// CR2
#define SPI_CR2_TXEIE (0x1 << 7)
#define SPI_CR2_RXNEIE (0x1 << 6)
#define SPI_CR2_ERRIE (0x1 << 5)
#define SPI_CR2_SSOE (0x1 << 2)
#define SPI_CR2_TXDMAEN (0x1 << 1)
#define SPI_CR2_RXDMAEN (0x1 << 0)

// SR
#define SPI_SR_BSY (0x1 << 7)
#define SPI_SR_OVR (0x1 << 6)
#define SPI_SR_MODF (0x1 << 5)
#define SPI_SR_CRCERR (0x1 << 4)
#define SPI_SR_UDR (0x1 << 3)
#define SPI_SR_CHSIDE (0x1 << 2)
#define SPI_SR_TXE (0x1 << 1)
#define SPI_SR_RXNE (0x1 << 0)

#define SPI_CR1_DFF_16Bit (0x1 << 11)
#define SPI_CR1_SSM (0x1 << 9)
#define SPI_CR1_SSI (0x1 << 8)
#define SPI_CR1_SPE (0x1 << 6)

#define SPI_CR1_BIDMODE (0x1 << 15)

// #define SPI_CR1_BR (0b100 << 3)
#define SPI_CR1_BR_16 (0b011 << 3)

#define SPI_MASK_ERROR (SPI_SR_OVR | SPI_SR_CRCERR)

typedef enum
{
    SPI_MODE_DISABLE = 0,
    SPI_MODE_POLLING,
    SPI_MODE_DMA
} SPI_Mode;

typedef enum
{
    SPI_CPOL_LOW,
    SPI_CPOL_HIGH
} SPI_CPOL_t;

typedef enum
{
    SPI_CPHA_1EDGE,
    SPI_CPHA_2EDGE
} SPI_CPHA_t;

typedef enum
{
    SPI_DATASIZE_8BIT,
    SPI_DATASIZE_16BIT
} SPI_DataSize_t;

typedef enum
{
    SPI_NSS_SOFT,
    SPI_NSS_HARD
} SPI_NSS_t;

typedef enum
{
    SPI_MASTER,
    SPI_SLAVE
} SPI_OperationMode_t;

typedef struct
{
    SPI_Type *spi;
    GPIO_PinConfig_t pin_sck;
    GPIO_PinConfig_t pin_miso;
    GPIO_PinConfig_t pin_mosi;
    GPIO_PinConfig_t pin_cs;
    DMA_Config *miso_dma;
    DMA_Config *mosi_dma;
    SPI_Mode miso_mode;
    SPI_Mode mosi_mode;
    uint32_t baud_rate;
    SPI_CPOL_t cpol;
    SPI_CPHA_t cpha;
    SPI_DataSize_t data_size;
    SPI_NSS_t nss;
    SPI_OperationMode_t spi_mode;
} SPI_Config_t;

typedef struct
{
    SPI_Type *spi;
    DMA_Config miso_dma;
    DMA_Config mosi_dma;
    SPI_Mode miso_mode;
    SPI_Mode mosi_mode;
    uint32_t baud_rate;
    SPI_CPOL_t cpol;
    SPI_CPHA_t cpha;
    SPI_DataSize_t data_size;
    SPI_NSS_t nss;
    SPI_OperationMode_t spi_mode;
} SPI_HandleTypeDef;

int setup_spi(SPI_Config_t *cfg, uint32_t spi_clk);

int send_data_spi_master(SPI_HandleTypeDef *cfg, uint8_t *data, uint32_t size);
int recv_data_spi_master(SPI_HandleTypeDef *cfg, uint8_t *data, uint32_t size);
