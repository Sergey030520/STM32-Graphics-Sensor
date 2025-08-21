#include "spi_board.h"
#include "gpio.h"
#include "rcc.h"
#include "board_pins.h"
#include <stdio.h>
#include <string.h>

#define SPI_TX_BUFFER_SIZE 512
#define SPI_RX_BUFFER_SIZE 512

SPI_Config_t spi_cfg = {0};

SPI_Buffer_t spi_tx_buffer;
SPI_Buffer_t spi_rx_buffer;

static uint8_t tx_buffer[SPI_TX_BUFFER_SIZE];
static uint8_t rx_buffer[SPI_RX_BUFFER_SIZE];

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
    DMA_Config dma_mosi_cfg = {
        .dma = DMA1_REG,
        .channel = CHANNEL_3,
        .direction = DMA_DIR_MEM_TO_PERIPH,
        .mem_size = DMA_DATASIZE_8BIT,
        .periph_size = DMA_DATASIZE_8BIT,
        .inc_mem = 1,
        .inc_periph = 0,
        .circular = 0,
    };

    DMA_Config dma_miso_cfg = {
        .dma = DMA1_REG,
        .channel = CHANNEL_2,
        .direction = DMA_DIR_PERIPH_TO_MEM,
        .mem_size = DMA_DATASIZE_8BIT,
        .periph_size = DMA_DATASIZE_8BIT,
        .inc_mem = 1,
        .inc_periph = 0,
        .circular = 0,
    };

    set_gpio_conf(&pin_sck_cfg);
    set_gpio_conf(&pin_miso_cfg);
    set_gpio_conf(&pin_mosi_cfg);
    set_gpio_conf(&pin_cs_cfg);

    spi_tx_buffer.buffer = tx_buffer;
    spi_tx_buffer.size = 0;
    spi_tx_buffer.pos = 0;
    spi_tx_buffer.complete = 0;

    spi_rx_buffer.buffer = rx_buffer;
    spi_rx_buffer.size = 0;
    spi_rx_buffer.pos = 0;
    spi_rx_buffer.complete = 0;

    // Конфигурация SPI
    spi_cfg.spi = SPI1_REG;
    spi_cfg.pin_cs = pin_cs_cfg;
    spi_cfg.pin_sck = pin_sck_cfg;
    spi_cfg.pin_miso = pin_miso_cfg;
    spi_cfg.pin_mosi = pin_mosi_cfg;
    spi_cfg.baud_rate = 20000000;
    spi_cfg.data_size = SPI_DATASIZE_8BIT;
    spi_cfg.cpha = SPI_CPHA_1EDGE;
    spi_cfg.cpol = SPI_CPOL_LOW;
    spi_cfg.nss = SPI_NSS_SOFT;
    spi_cfg.spi_mode = SPI_MASTER;
    spi_cfg.mosi_dma = &dma_mosi_cfg;
    spi_cfg.mosi_mode = SPI_MODE_POLLING;
    spi_cfg.miso_dma = NULL;
    spi_cfg.miso_mode = SPI_MODE_DISABLE;
    spi_cfg.tx_buff = &spi_tx_buffer;
    spi_cfg.rx_buff = &spi_rx_buffer;

    return setup_spi(&spi_cfg, freq.APB2_Freq);
}

int tft_spi_send(uint8_t *data, uint32_t size)
{
    if (!data || size == 0 || size > SPI_TX_BUFFER_SIZE)
        return -1;

    memcpy(spi_tx_buffer.buffer, data, size);

    spi_tx_buffer.size = size;
    spi_tx_buffer.pos = 0;
    spi_tx_buffer.complete = 0;

    // CS low
    reset_pin_gpio(spi_cfg.pin_cs.gpiox, spi_cfg.pin_cs.pin);

    // Передача через DMA
    send_data_spi_master(&spi_cfg);

    // CS high
    set_pin_gpio(spi_cfg.pin_cs.gpiox, spi_cfg.pin_cs.pin);
    return 0;
}

int tft_spi_recv(uint8_t *data, uint32_t size)
{
    if (!data || size == 0 || size > SPI_RX_BUFFER_SIZE)
        return -1;

    spi_rx_buffer.size = size;
    spi_rx_buffer.pos = 0;
    spi_rx_buffer.complete = 0;

    int status = recv_data_spi_master(&spi_cfg);
    if (status != 0)
        return status;

    for (uint32_t i = 0; i < size; i++)
    {
        data[i] = spi_rx_buffer.buffer[i];
    }

    return 0;
}