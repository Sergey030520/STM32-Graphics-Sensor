#include "spi.h"
#include "board_pins.h"
#include "dma.h"
#include <stdio.h>
#include "rcc.h"

uint16_t master_buff[128];

int master_idx = 0;
int isSendDataDMA = 0;

uint32_t calculate_SPI_BRR(uint32_t spi_clk, uint32_t baud_rate);
int spi_send_polling(SPI_Config_t *cfg);
int spi_send_dma(SPI_Config_t *cfg);
int spi_recv_polling(SPI_Config_t *cfg);
int spi_recv_dma(SPI_Config_t *cfg);

uint32_t calculate_SPI_BRR(uint32_t spi_clk, uint32_t baud_rate)
{
    uint32_t div = spi_clk / baud_rate;

    if (div <= 2)
        return SPI_BAUDRATE_DIV2;
    if (div <= 4)
        return SPI_BAUDRATE_DIV4;
    if (div <= 8)
        return SPI_BAUDRATE_DIV8;
    if (div <= 16)
        return SPI_BAUDRATE_DIV16;
    if (div <= 32)
        return SPI_BAUDRATE_DIV32;
    if (div <= 64)
        return SPI_BAUDRATE_DIV64;
    if (div <= 128)
        return SPI_BAUDRATE_DIV128;
    return SPI_BAUDRATE_DIV256;
}

int setup_spi(SPI_Config_t *spi_cfg, uint32_t spi_clk)
{
    if (spi_cfg == NULL)
    {
        return -1;
    }
    if (spi_cfg->spi == NULL)
    {
        return -2;
    }
    // setup CSS
    set_gpio_conf(&spi_cfg->pin_sck);
    // setup CS
    set_gpio_conf(&spi_cfg->pin_cs);
    // setup MISO
    set_gpio_conf(&spi_cfg->pin_miso);
    // setup MOSI
    set_gpio_conf(&spi_cfg->pin_mosi);

    SPI_Type *spi = spi_cfg->spi;

    // clear settings reg
    spi->CR1 = 0;
    spi->CR2 = 0;

    uint32_t spi_div = calculate_SPI_BRR(spi_clk, spi_cfg->baud_rate);
    spi->CR1 |= SPI_CR1_BR(spi_div);

    if (spi_cfg->cpol == SPI_CPOL_HIGH)
        spi->CR1 |= SPI_CR1_CPOL;
    if (spi_cfg->cpha == SPI_CPHA_2EDGE)
        spi->CR1 |= SPI_CR1_CPHA;

    if (spi_cfg->data_size == SPI_DATASIZE_16BIT)
    {
        spi->CR1 |= SPI_CR1_DFF;
    }

    if (spi_cfg->miso_dma)
    {

        if (spi_cfg->data_size == SPI_DATASIZE_16BIT)
            spi_cfg->miso_dma->mem_size = DMA_DATASIZE_16BIT;
        spi_cfg->miso_dma->peripheral_addr = (uint32_t)&spi->DR;
        dma_init(spi_cfg->miso_dma);
    }
    if (spi_cfg->mosi_dma)
    {
        if (spi_cfg->data_size == SPI_DATASIZE_16BIT)
            spi_cfg->mosi_dma->mem_size = DMA_DATASIZE_16BIT;

        spi_cfg->mosi_dma->peripheral_addr = (uint32_t)&spi->DR;
        dma_init(spi_cfg->mosi_dma);
    }
    if (spi_cfg->spi_mode == SPI_MASTER)
    {
        spi->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;
    }
    else // Slave
    {
        spi->CR1 &= ~SPI_CR1_MSTR;
        if (spi_cfg->nss == SPI_NSS_HARD)
            spi->CR1 &= ~SPI_CR1_SSM;
        else
            spi->CR1 |= SPI_CR1_SSM;
    }

    // Включаем SPI
    spi->CR1 |= SPI_CR1_SPE;

    return 0;
}

int send_data_spi_master(SPI_Config_t *cfg)
{
    if (!cfg || !cfg->tx_buff || !cfg->tx_buff->buffer || cfg->tx_buff->size == 0)
        return -1;

    if (cfg->mosi_mode == SPI_MODE_POLLING)
        return spi_send_polling(cfg);
    else if (cfg->mosi_mode == SPI_MODE_DMA)
        return spi_send_dma(cfg);

    return -2;
}

int spi_send_polling(SPI_Config_t *cfg)
{
    if (!cfg || !cfg->tx_buff || !cfg->tx_buff->buffer || cfg->tx_buff->size == 0)
        return -1;

    SPI_Type *spi = cfg->spi;
    uint16_t dummy;
    SPI_Buffer_t *buff = cfg->tx_buff;

    for (uint32_t i = 0; i < buff->size; i++)
    {
        while (!(spi->SR & SPI_SR_TXE))
            ;

        if (cfg->data_size == SPI_DATASIZE_8BIT)
            spi->DR = (uint8_t)(buff->buffer[i] & 0xFF);
        else
            spi->DR = buff->buffer[i];

        while (!(spi->SR & SPI_SR_RXNE))
            ;
        dummy = spi->DR;
        (void)dummy;
    }

    while (spi->SR & SPI_SR_BSY)
        ;

    buff->complete = 1;

    return 0;
}

int spi_send_dma(SPI_Config_t *cfg)
{
    if (!cfg || !cfg->mosi_dma || !cfg->tx_buff ||
        !cfg->tx_buff->buffer || cfg->tx_buff->size == 0)
        return -1;

    cfg->tx_buff->pos = 0;
    cfg->tx_buff->complete = 0;

    uint32_t tmp = cfg->spi->SR;
    tmp = cfg->spi->DR;
    (void)tmp;

    cfg->spi->CR2 |= SPI_CR2_TXDMAEN;

    dma_set_memory(cfg->mosi_dma,
                   (uint32_t)cfg->tx_buff->buffer,
                   cfg->tx_buff->size);

    dma_start(cfg->mosi_dma);


    while (!dma_transfer_complete(cfg->mosi_dma))
        ;

    uint32_t timeout = 100000;
    while (!(cfg->spi->SR & SPI_SR_TXE) && timeout--)
        ;

    timeout = 100000;
    while ((cfg->spi->SR & SPI_SR_BSY) && timeout--)
        ;

    // 6) dummy-чтение для гашения флагов
    uint32_t dr = cfg->spi->DR;
    uint32_t sr = cfg->spi->SR;
    (void)sr;
    (void)dr;

    // 7) выключить запросы
    cfg->spi->CR2 &= ~SPI_CR2_TXDMAEN;

    cfg->tx_buff->complete = 1;
    return 0;
}

int recv_data_spi_master(SPI_Config_t *cfg)
{
    if (!cfg || !cfg->rx_buff || !cfg->rx_buff->buffer || cfg->rx_buff->size == 0)
        return -1;

    if (cfg->miso_mode == SPI_MODE_POLLING)
        return spi_recv_polling(cfg);
    else if (cfg->miso_mode == SPI_MODE_DMA)
        return spi_recv_dma(cfg);

    return -2;
}

int spi_recv_polling(SPI_Config_t *cfg)
{
    if (!cfg || !cfg->rx_buff || !cfg->rx_buff->buffer || cfg->rx_buff->size == 0)
        return -1;

    SPI_Type *spi = cfg->spi;
    SPI_Buffer_t *buff = cfg->rx_buff;

    for (uint32_t i = 0; i < buff->size; i++)
    {
        while (!(spi->SR & SPI_SR_TXE))
            ;

        if (cfg->data_size == SPI_DATASIZE_8BIT)
            spi->DR = 0xFF;
        else
            spi->DR = 0xFFFF;

        while (!(spi->SR & SPI_SR_RXNE))
            ;

        if (cfg->data_size == SPI_DATASIZE_8BIT)
            buff->buffer[i] = spi->DR & 0xFF;
        else
            buff->buffer[i] = spi->DR;
    }

    while (spi->SR & SPI_SR_BSY)
        ;

    buff->complete = 1;

    return 0;
}

int spi_recv_dma(SPI_Config_t *cfg)
{
    if (!cfg || !cfg->miso_dma || !cfg->rx_buff || !cfg->rx_buff->buffer || cfg->rx_buff->size == 0)
        return -1;

    cfg->rx_buff->pos = 0;
    cfg->rx_buff->complete = 0;

    // Включаем RX DMA в SPI
    cfg->spi->CR2 |= SPI_CR2_RXDMAEN;

    dma_set_memory(cfg->miso_dma, (uint32_t)cfg->rx_buff->buffer, cfg->rx_buff->size);
    dma_start(cfg->miso_dma);

    // Ждем окончания приёма
    while (!dma_transfer_complete(cfg->miso_dma))
        ;

    cfg->spi->CR2 &= ~SPI_CR2_RXDMAEN;
    cfg->rx_buff->complete = 1;

    return 0;
}