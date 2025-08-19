#include "usart.h"
#include "memory_map.h"
#include "board_pins.h"
#include "led.h"
#include "rcc.h"
#include <stdio.h>


#define DMA_USART1_CHANNEL 4

UART_HandleTypeDef huart;

void send_byte_usart(uint8_t byte)
{
    USART_Type *uartx = (USART_Type *)huart.usart;
    while (!(uartx->SR & USART_SR_TXE))
        ;
    uartx->DR = byte;
    while (!(uartx->SR & USART_SR_TC))
        ;
}

void send_data_usart(uint8_t *message, uint16_t length)
{
    if (huart.tx_mode == UART_MODE_DMA)
    {
        dma_send(&huart.dma_tx, message, length);
    }
    else
    {
        for (int i = 0; i < length; ++i)
        {
            send_byte_usart(message[i]);
        }
    }
}


uint32_t calculate_USART_BRR(uint32_t usart_clk, uint32_t baud_rate)
{
    float usartdiv = (float)usart_clk / (baud_rate * 16);
    uint16_t div_mantissa = (uint16_t)usartdiv;
    uint16_t div_fraction = (uint16_t)((usartdiv - div_mantissa) * 16 + 0.5f);
    return (uint32_t)USART_BRR(div_mantissa, div_fraction);
}

void setup_uart(UART_Config_t *cfg, uint32_t usart_clk)
{
    if (cfg == NULL)
    {
        return;
    }

    huart.usart = cfg->usart;
    huart.tx_mode = cfg->tx_mode;
    huart.rx_mode = cfg->rx_mode;
    huart.dma_tx = cfg->dma_tx;
    huart.dma_rx = cfg->dma_rx;

    if (cfg->tx_port)
        set_gpio_conf(cfg->tx_port);
    if (cfg->rx_port)
        set_gpio_conf(cfg->rx_port);

    USART_Type *usartx = cfg->usart;

    usartx->CR1 &= ~USART_CR1_UE;

    usartx->BRR = calculate_USART_BRR(usart_clk, cfg->baud_rate);

    usartx->CR1 = 0;
    usartx->CR2 = 0;
    usartx->CR3 = 0;

    if (cfg->tx_mode == UART_MODE_DMA)
    {
        usartx->CR3 |= USART_CR3_DMAT;
        cfg->dma_tx.peripheral_addr = (uint32_t) &usartx->DR;
        dma_init(&cfg->dma_tx);
    }
    if (cfg->rx_mode == UART_MODE_DMA)
    {
        usartx->CR3 |= USART_CR3_DMAR;
        cfg->dma_tx.peripheral_addr = (uint32_t) &usartx->DR;
        dma_init(&cfg->dma_rx);
    }

    if (cfg->tx_mode != UART_MODE_DISABLE)
    {
        usartx->CR1 |= USART_CR1_TE;
    }
    if (cfg->rx_mode != UART_MODE_DISABLE)
    {
        usartx->CR1 |= USART_CR1_RE;
    }

    // Включаем сам USART
    usartx->CR1 |= USART_CR1_UE;

    volatile uint32_t tmp;
    tmp = usartx->SR;
    tmp = usartx->DR;
    (void)tmp;
}


void USART1_IRQHandler(void)
{
    // uint8_t byte = 0;
    // if (usart1->SR & USART_SR_RXNE)
    // {
    //     byte = usart1->DR;
    //     // send_byte_usart(byte);
    // }
}


