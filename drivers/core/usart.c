#include "usart.h"
#include "memory_map.h"
#include "board_pins.h"
#include "led.h"
#include "rcc.h"
#include <stdio.h>
#include <string.h>



void send_byte_usart(UART_HandleTypeDef *huart, uint8_t byte)
{
    USART_Type *uartx = huart->usart;
    while (!(uartx->SR & USART_SR_TXE))
        ;
    uartx->DR = byte;
    while (!(uartx->SR & USART_SR_TC))
        ;
}

void send_data_usart(UART_HandleTypeDef *huart, uint8_t *message, uint16_t length)
{
    if (huart->tx_mode == UART_MODE_DMA)
    {
        while (!dma_transfer_complete(huart->dma_tx.dma, huart->dma_tx.channel))
            ; 
        huart->dma_tx.mem_addr = (uint32_t)message;
        huart->dma_tx.length = length;
        dma_init(&huart->dma_tx);
        dma_start(&huart->dma_tx);
        while (!dma_transfer_complete(huart->dma_tx.dma, huart->dma_tx.channel))
            ; 
    }
    else
    {
        for (uint16_t i = 0; i < length; i++)
        {
            send_byte_usart(huart, message[i]);
        }
    }
}

uint32_t calculate_USART_BRR(uint32_t usart_clk, uint32_t baud_rate)
{
    uint32_t usartdiv = (usart_clk + (baud_rate / 2)) / baud_rate;
    uint32_t mantissa = usartdiv / 16;
    uint32_t fraction = usartdiv - (mantissa * 16);

    return USART_BRR(mantissa, fraction);
}

void setup_uart(UART_Config_t *cfg, uint32_t usart_clk)
{
    if (cfg == NULL)
        return;

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

    // Настройка DMA
    if (cfg->tx_mode == UART_MODE_DMA)
    {
        usartx->CR3 |= USART_CR3_DMAT;
        cfg->dma_tx.peripheral_addr = (uint32_t)&usartx->DR;
    }
    if (cfg->rx_mode == UART_MODE_DMA)
    {
        usartx->CR3 |= USART_CR3_DMAR;
        cfg->dma_rx.peripheral_addr = (uint32_t)&usartx->DR; // исправлено
    }

    // Включаем передатчик/приёмник
    if (cfg->tx_mode != UART_MODE_DISABLE)
        usartx->CR1 |= USART_CR1_TE;
    if (cfg->rx_mode != UART_MODE_DISABLE)
        usartx->CR1 |= USART_CR1_RE;

    // Включаем USART
    usartx->CR1 |= USART_CR1_UE;

    // Сброс флагов
    volatile uint32_t tmp;
}

// void USART1_IRQHandler(void)
// {
//     // uint8_t byte = 0;
//     // if (usart1->SR & USART_SR_RXNE)
//     // {
//     //     byte = usart1->DR;
//     //     // send_byte_usart(byte);
//     // }
// }
