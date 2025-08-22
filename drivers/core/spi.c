#include "spi.h"
#include "board_pins.h"
#include "dma.h"
#include <stdio.h>
#include "rcc.h"


uint32_t calculate_SPI_BRR(uint32_t spi_clk, uint32_t baud_rate);
int spi_send_polling(SPI_Config_t *cfg, uint8_t *data, uint32_t size);
int spi_send_dma(SPI_Config_t *cfg, uint8_t *data, uint32_t size);
int spi_recv_polling(SPI_Config_t *cfg, uint8_t *data, uint32_t size);
int spi_recv_dma(SPI_Config_t *cfg, uint8_t *data, uint32_t size);

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

    if (spi_cfg->miso_dma)
    {

        if (spi_cfg->data_size == SPI_DATASIZE_16BIT)
            spi_cfg->miso_dma->mem_size = DMA_DATASIZE_16BIT;
        spi_cfg->miso_dma->peripheral_addr = (uint32_t)&spi->DR;
        spi->CR2 |= SPI_CR2_RXDMAEN;
    }
    if (spi_cfg->mosi_dma)
    {
        if (spi_cfg->data_size == SPI_DATASIZE_16BIT)
            spi_cfg->mosi_dma->mem_size = DMA_DATASIZE_16BIT;

        spi_cfg->mosi_dma->peripheral_addr = (uint32_t)&spi->DR;
        spi->CR2 |= SPI_CR2_TXDMAEN;
    }

    // Включаем SPI
    spi->CR1 |= SPI_CR1_SPE;

    if (!(spi->SR & SPI_SR_TXE))
        spi->DR = 0;

    return 0;
}

int send_data_spi_master(SPI_Config_t *cfg, uint8_t *data, uint32_t size)
{
    if (!cfg || !data || size == 0)
        return -1;

    if (cfg->mosi_mode == SPI_MODE_POLLING)
        return spi_send_polling(cfg, data, size);
    else if (cfg->mosi_mode == SPI_MODE_DMA)
        return spi_send_dma(cfg, data, size);

    return -2;
}

int spi_send_polling(SPI_Config_t *cfg, uint8_t *data, uint32_t size)
{
    if (!cfg || !data || size == 0)
        return -1;

    SPI_Type *spi = cfg->spi;
    uint16_t dummy;

    for (uint32_t i = 0; i < size; i++)
    {
        while (!(spi->SR & SPI_SR_TXE))
            ;

        if (cfg->data_size == SPI_DATASIZE_8BIT)
            spi->DR = data[i];
        else
            spi->DR = ((uint16_t *)data)[i];

        while (!(spi->SR & SPI_SR_RXNE))
            ;
        dummy = spi->DR; // гасим RXNE
        (void)dummy;
    }

    while (spi->SR & SPI_SR_BSY)
        ;

    return 0;
}

int spi_send_dma(SPI_Config_t *cfg, uint8_t *data, uint32_t size)
{
    if (!cfg || !cfg->mosi_dma || !data || size == 0)
        return -1;


    SPI_Type *spi = cfg->spi;

    while (!dma_transfer_complete(cfg->mosi_dma->dma, cfg->mosi_dma->channel))
        ;


    // Очистка флагов SPI
    volatile uint32_t tmp = spi->SR;
    tmp = spi->DR;
    (void)tmp;

    // Настройка DMA
    cfg->mosi_dma->mem_addr = (uint32_t)data;
    cfg->mosi_dma->length = size;

    dma_init(cfg->mosi_dma);
    dma_start(cfg->mosi_dma);

    return 0;
}

int recv_data_spi_master(SPI_Config_t *cfg, uint8_t *data, uint32_t size)
{
    if (!cfg || !data || size == 0)
        return -1;

    if (cfg->miso_mode == SPI_MODE_POLLING)
        return spi_recv_polling(cfg, data, size);
    else if (cfg->miso_mode == SPI_MODE_DMA)
        return spi_recv_dma(cfg, data, size);

    return -2;
}

int spi_recv_polling(SPI_Config_t *cfg, uint8_t *data, uint32_t size)
{
    if (!cfg || !data || size == 0)
        return -1;

    SPI_Type *spi = cfg->spi;

    for (uint32_t i = 0; i < size; i++)
    {
        // ждем, пока TXE станет доступен
        while (!(spi->SR & SPI_SR_TXE))
            ;

        // отправляем заглушку, чтобы запустить тактирование и получить байт
        if (cfg->data_size == SPI_DATASIZE_8BIT)
            spi->DR = 0xFF;
        else
            spi->DR = 0xFFFF;

        // ждем прихода данных
        while (!(spi->SR & SPI_SR_RXNE))
            ;

        // читаем данные
        if (cfg->data_size == SPI_DATASIZE_8BIT)
            data[i] = (uint8_t)(spi->DR & 0xFF);
        else
            ((uint16_t *)data)[i] = (uint16_t)(spi->DR);
    }

    // ждем, пока SPI не освободится
    while (spi->SR & SPI_SR_BSY)
        ;

    return 0;
}

int spi_recv_dma(SPI_Config_t *cfg, uint8_t *data, uint32_t size)
{
    if (!cfg || !cfg->miso_dma || !data || size == 0)
        return -1;

    dma_init(cfg->miso_dma);
    dma_start(cfg->miso_dma);

    // Ждем окончания передачи
    while (!dma_transfer_complete(cfg->miso_dma->dma, cfg->miso_dma->channel))
        ;

    return 0;
}
