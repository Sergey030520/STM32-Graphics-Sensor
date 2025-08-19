#include "spi.h"
#include "configuration.h"
#include "usart.h"
#include "dma.h"

#define SPI_NSS_Port 4
#define SPI_SCK_Port 5
#define SPI_MISO_Port 6
#define SPI_MOSI_Port 7

SPI_Type *spi_master = SPI1_Reg;

uint16_t master_buff[128];
int master_idx = 0;
int isSendDataDMA = 0;

int init_spi_master()
{
    // reset
    gpioa->CRL &= ~(GPIO_MASK_PORT_CRL(SPI_NSS_Port) | GPIO_MASK_PORT_CRL(SPI_SCK_Port) | GPIO_MASK_PORT_CRL(SPI_MISO_Port) | GPIO_MASK_PORT_CRL(SPI_MOSI_Port));

    // setup CSS
    gpioa->CRL |= GPIO_CRL_CNF_MODE(SPI_SCK_Port, GPIO_MODE_SPEED_50MHz,
                                    GPIO_AF_PUSH_PULL);

    // setup MOSI
    gpioa->CRL |= GPIO_CRL_CNF_MODE(SPI_MOSI_Port, GPIO_MODE_SPEED_50MHz,
                                    GPIO_AF_PUSH_PULL);

    spi_master->CR1 = 0;
    spi_master->CR2 = 0;
    spi_master->CR2 = SPI_CR2_TXDMAEN;
    spi_master->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | (0x3 << 3) | SPI_CR1_CPOL | SPI_CR1_CPHA;
    spi_master->CR1 |= SPI_CR1_SPE;

    dma1->IFCR = DMA_IFCR_CTCIFx(CHANNEL_3);

    return 0;
}


void SPI1_IRQHandler()
{
    if (spi_master->SR & SPI_SR_BSY)
    {

        send_text("{SPI1: BSY}", 11);
    }
    if (spi_master->SR & SPI_MASK_ERROR)
    {
        send_text("{error SPI1: 1}", 15);
    }
    if (spi_master->SR & SPI_SR_RXNE)
    {
        master_buff[master_idx] = spi_master->DR;
        if (master_idx == 128)
        {
            master_idx = 0;
        }
        send_text("{SPI1 send text!}", 15);
    }
}

void send_data_spi(uint8_t *data, uint32_t size)
{
   spi_master->DR  = data[0];
}

uint16_t recv_data_spi(SPI_Type *spi)
{
    spi->DR = 0;
    while (!(spi->SR & SPI_SR_TXE))
        ;
    return spi->DR;
}

void send_data_spi_dma(uint8_t *message, uint16_t length)
{
    int channel = CHANNEL_3 - 1;
    isSendDataDMA = 0;
    dma1->channels[channel].CCR = 0;
    dma1->channels[channel].CNDTR = length;
    dma1->channels[channel].CPAR = (uint32_t)(&spi_master->DR);
    dma1->channels[channel].CMAR = (uint32_t)message;
    dma1->channels[channel].CCR |= DMA_MSIZE_8BITS | DMA_PSIZE_8BITS | DMA_MINC | DMA_DIR_MEMORY | DMA_TCIE;
    dma1->channels[channel].CCR |= DMA_EN;

    // while (!isSendDataDMA)
    //     ;
    while(!(dma1->ISR & TCIFx(CHANNEL_3)));
    
    dma1->IFCR &= DMA_IFCR_CTCIFx(CHANNEL_3);
    dma1->channels[channel].CCR &= ~DMA_EN;
}

void recive_data_spi1_dma(uint8_t *buff, uint16_t length)
{
    DMA_Channel channel = CHANNEL_2;
    dma1->channels[channel].CCR = 0;
    dma1->channels[channel].CNDTR = length;
    dma1->channels[channel].CPAR = (uint32_t)&spi_master->DR;
    dma1->channels[channel].CMAR = (uint32_t)buff;
    dma1->channels[channel].CCR |= DMA_MSIZE_8BITS | DMA_PSIZE_8BITS | DMA_MINC | DMA_DIR_PERIPHERAL | DMA_TEIE | DMA_TCIE | DMA_HTIE;
    dma1->channels[channel].CCR |= DMA_EN;
}

void DMA1_Channel3_IRQHandler(void)
{
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
}