#include "spi.h"
#include "board_pins.h"
#include "dma.h"
#include <stdio.h>
#include "rcc.h"

uint16_t master_buff[128];

int master_idx = 0;
int isSendDataDMA = 0;



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
        spi->CR1 |= SPI_CR1_DFF;

    if (spi_cfg->miso_dma)
    {
        spi->CR2 |= SPI_CR2_RXDMAEN;
        dma_init(spi_cfg->miso_dma);
    }
    if (spi_cfg->mosi_dma)
    {
        spi->CR2 |= SPI_CR2_TXDMAEN;
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

int send_data_spi_master(SPI_Config_t *spi_cfg, uint16_t *data, uint32_t size)
{
    if (spi_cfg == NULL)
        return -1;

    SPI_Type *spi = spi_cfg->spi;
    uint16_t dummy = 0;
    for (uint32_t i = 0; i < size; i++)
    {
        while (!(spi->SR & SPI_SR_TXE))
            ;

        if (spi_cfg->data_size == SPI_DATASIZE_8BIT)
        {
            spi->DR = (uint8_t)(data[i] & 0xFF);
        }
        else
        {
            spi->DR = data[i];
        }

        while (!(spi->SR & SPI_SR_RXNE))
            ;
        dummy = spi->DR; // читаем для очистки флага RXNE
        (void)dummy;
    }

    while (spi->SR & SPI_SR_BSY)
        ;
}

int recv_data_spi_master(SPI_Config_t *spi_cfg, uint16_t *buffer, uint32_t size)
{
    if (spi_cfg == NULL)
        return -1;

    SPI_Type *spi = spi_cfg->spi;
    for (uint32_t i = 0; i < size; i++)
    {
        while (!(spi->SR & SPI_SR_TXE))
            ;
        if (spi_cfg->data_size == SPI_DATASIZE_8BIT)
            spi->DR = 0xFF;
        else
            spi->DR = 0xFFFF;

        while (!(spi->SR & SPI_SR_RXNE))
            ;
        if (spi_cfg->data_size == SPI_DATASIZE_8BIT)
            buffer[i] = spi->DR & 0xFF;
        else
            buffer[i] = spi->DR;
    }

    while (spi->SR & SPI_SR_BSY)
        ;
}

int spi_master_send_dma(SPI_Config_t *cfg)
{
    if (!cfg || !cfg->mosi_dma || !cfg->tx_buff || !cfg->tx_buff->buffer || cfg->tx_buff->size == 0)
        return -1;

    SPI_Buffer_t *tx = cfg->tx_buff;
    tx->pos = 0;
    tx->complete = 0;

    cfg->spi->CR2 |= SPI_CR2_TXDMAEN;

    dma_set_memory(cfg->mosi_dma, (uint32_t)tx->buffer, tx->size);
    dma_start(cfg->mosi_dma);

    while (!dma_transfer_complete(cfg->mosi_dma));

    cfg->spi->CR2 &= ~SPI_CR2_TXDMAEN;

    tx->complete = 1;
}

int spi_master_recv_dma(SPI_Config_t *cfg)
{
    if (!cfg || !cfg->miso_dma || !cfg->rx_buff || !cfg->rx_buff->buffer || cfg->rx_buff->size == 0)
        return -1;

    SPI_Buffer_t *rx = cfg->rx_buff;
    rx->pos = 0;
    rx->complete = 0;

    cfg->spi->CR2 |= SPI_CR2_RXDMAEN;

    dma_set_memory(cfg->miso_dma, (uint32_t)rx->buffer, rx->size);
    dma_start(cfg->miso_dma);

    while (!dma_transfer_complete(cfg->miso_dma));

    cfg->spi->CR2 &= ~SPI_CR2_RXDMAEN;

    rx->complete = 1;
    return 0;
}


// void send_data_spi_dma(uint8_t *message, uint16_t length)
// {
//     int channel = CHANNEL_3 - 1;
//     isSendDataDMA = 0;
//     dma1->channels[channel].CCR = 0;
//     dma1->channels[channel].CNDTR = length;
//     dma1->channels[channel].CPAR = (uint32_t)(&spi_master->DR);
//     dma1->channels[channel].CMAR = (uint32_t)message;
//     dma1->channels[channel].CCR |= DMA_MSIZE_8BITS | DMA_PSIZE_8BITS | DMA_MINC | DMA_DIR_MEMORY | DMA_TCIE;
//     dma1->channels[channel].CCR |= DMA_EN;

//     // while (!isSendDataDMA)
//     //     ;
//     while (!(dma1->ISR & TCIFx(CHANNEL_3)))
//         ;

//     dma1->IFCR &= DMA_IFCR_CTCIFx(CHANNEL_3);
//     dma1->channels[channel].CCR &= ~DMA_EN;
// }

// void recive_data_spi1_dma(uint8_t *buff, uint16_t length)
// {
//     DMA_Channel channel = CHANNEL_2;
//     dma1->channels[channel].CCR = 0;
//     dma1->channels[channel].CNDTR = length;
//     dma1->channels[channel].CPAR = (uint32_t)&spi_master->DR;
//     dma1->channels[channel].CMAR = (uint32_t)buff;
//     dma1->channels[channel].CCR |= DMA_MSIZE_8BITS | DMA_PSIZE_8BITS | DMA_MINC | DMA_DIR_PERIPHERAL | DMA_TEIE | DMA_TCIE | DMA_HTIE;
//     dma1->channels[channel].CCR |= DMA_EN;
// }

// void DMA1_Channel3_IRQHandler(void)
// {
// ledOn(1);
// DMA_Channel channel = CHANNEL_3;
// if (dma->ISR & TEIFx(channel)) // Check tranfer error
// {
//     dma->IFCR |= DMA_IFCR_CTEIFx(channel);
// }
// if (dma->ISR & HTIFx(channel)) // Check half transfer
// {
//     dma->IFCR |= CHTIFx(channel);
// }
// if (dma->ISR & TCIFx(channel)) // Check transfer complete
// {
//     isSendDataDMA = 1;
//     dma->IFCR  |= DMA_IFCR_CTCIFx(channel);
// }
// if (dma->ISR & GIFx(channel)) // Check global interrupt
// {
//     dma->IFCR |= DMA_IFCR_CGIFx(channel);
// }
// }