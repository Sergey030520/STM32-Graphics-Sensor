#include "spi_board.h"
#include "gpio.h"
#include "rcc.h"
#include "board_pins.h"
#include <stdio.h>
#include <string.h>
#include "log.h"

#define SPI_TX_BUFFER_SIZE 512
#define SPI_RX_BUFFER_SIZE 512

SPI_Config_t spi_cfg = {0};
DMA_Config dma_mosi_cfg = {0};
// DMA_Config dma_miso_cfg = {0};

int init_spi()
{
    RCC_Frequencies freq = {0};
    get_clock_frequencies(&freq);

    enable_and_reset_rcc(RCC_BUS_APB2, RCC_APB2ENR_SPI1EN);

    // Настройка пинов SPI
    GPIO_PinConfig_t pin_sck_cfg = {
        .gpiox = SPI1_SCK_PORT,
        .pin = SPI1_SCK_PIN,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_AF_PP,
        .af_remap = 0,
    };

    GPIO_PinConfig_t pin_cs_cfg = {
        .gpiox = SPI1_CS_PORT,
        .pin = SPI1_CS_PIN,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_GP_PP,
        .af_remap = 0,
    };

    GPIO_PinConfig_t pin_miso_cfg = {
        .gpiox = SPI1_MISO_PORT,
        .pin = SPI1_MISO_PIN,
        .speed = GPIO_MODE_INPUT,
        .pin_mode = GPIO_INPUT_FLOATING,
        .af_remap = 0,
    };

    GPIO_PinConfig_t pin_mosi_cfg = {
        .gpiox = SPI1_MOSI_PORT,
        .pin = SPI1_MOSI_PIN,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_AF_PP,
        .af_remap = 0,
    };

    // Настройка DMA

    dma_mosi_cfg.dma = (DMA_Type*)DMA1_REG;
    dma_mosi_cfg.channel = CHANNEL_3;
    dma_mosi_cfg.direction = DMA_DIR_MEM_TO_PERIPH;
    dma_mosi_cfg.mem_size = DMA_DATASIZE_8BIT;
    dma_mosi_cfg.periph_size = DMA_DATASIZE_8BIT;
    dma_mosi_cfg.inc_mem = 1;
    dma_mosi_cfg.inc_periph = 0;
    dma_mosi_cfg.circular = 0;

    // dma_miso_cfg.dma = (DMA_Type*)DMA1_REG;
    // dma_miso_cfg.channel = CHANNEL_2;
    // dma_miso_cfg.direction = DMA_DIR_MEM_TO_PERIPH;
    // dma_miso_cfg.mem_size = DMA_DATASIZE_8BIT;
    // dma_miso_cfg.periph_size = DMA_DATASIZE_8BIT;
    // dma_miso_cfg.inc_mem = 1;
    // dma_miso_cfg.inc_periph = 0;
    // dma_miso_cfg.circular = 0;

    set_gpio_conf(&pin_sck_cfg);
    set_gpio_conf(&pin_miso_cfg);
    set_gpio_conf(&pin_mosi_cfg);
    set_gpio_conf(&pin_cs_cfg);

    // Конфигурация SPI
    spi_cfg.spi = (SPI_Type *)SPI1_REG;
    spi_cfg.pin_cs = pin_cs_cfg;
    spi_cfg.pin_sck = pin_sck_cfg;
    spi_cfg.pin_miso = pin_miso_cfg;
    spi_cfg.pin_mosi = pin_mosi_cfg;
    spi_cfg.baud_rate = 9000000;
    spi_cfg.data_size = SPI_DATASIZE_8BIT;
    spi_cfg.cpha = SPI_CPHA_1EDGE;
    spi_cfg.cpol = SPI_CPOL_LOW;
    spi_cfg.nss = SPI_NSS_SOFT;
    spi_cfg.spi_mode = SPI_MASTER;
    spi_cfg.mosi_dma = &dma_mosi_cfg;
    spi_cfg.mosi_mode = SPI_MODE_DMA;
    spi_cfg.miso_dma = NULL;
    spi_cfg.miso_mode = SPI_MODE_DISABLE;
    return setup_spi(&spi_cfg, freq.APB2_Freq);
}

int tft_spi_send(uint8_t *data, uint32_t size)
{
    if (!data || size == 0 || size > SPI_TX_BUFFER_SIZE)
        return -1;


    int status = send_data_spi_master(&spi_cfg, data, size);

    return status;
}

int tft_spi_recv(uint8_t *data, uint32_t size)
{
    if (!data || size == 0 || size > SPI_RX_BUFFER_SIZE)
        return -1;


    int status = recv_data_spi_master(&spi_cfg, data, size);

    return status;
}
