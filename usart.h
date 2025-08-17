#include <stdint.h>

#pragma once

#define USART1_REG 0x40013800U
#define USART2_REG 0x40004400U

typedef struct
{
    uint32_t SR;
    uint32_t DR;
    uint32_t BRR;
    uint32_t CR1;
    uint32_t CR2;
    uint32_t CR3;
    uint32_t GTPR;
} USARTx;

#define USART_STOP_1BIT (0b00 << 12)
#define USART_CLKEN (0x1 << 11)
#define USART_M_9BIT (0x1 << 13)
#define USART_PS_ODD (0x1 << 9)
#define USART_PEIE (0x1 << 8)
#define USART_TXEIE (0x1 << 7)
#define USART_TCIE (0x1 << 6)
#define USART_RXNEIE (0x1 << 5)
#define USART_IDLEIE (0x1 << 4)


#define USART_TE (0x1 << 3)
#define USART_RE (0x1 << 2)
#define USART_UE (0x1 << 13)

// SR
#define USART_SR_PE (0x1 << 0)
#define USART_SR_FE (0x1 << 1)
#define USART_SR_NE (0x1 << 2)
#define USART_SR_ORE (0x1 << 3)
#define USART_SR_RXNE (0x1 << 5)
#define USART_SR_TC (0x1 << 6)
#define USART_SR_TXE (0x1 << 7)

// CR3

#define USART_DMAT (0x1 << 7)
#define USART_DMAR (0x1 << 6)
#define USART_EIE (0x1 << 0)


extern USARTx *usart1;

void init_uart();
void send(uint8_t byte);
void send_text(char *buff, int length);
void USART1_IRQHandler(void);