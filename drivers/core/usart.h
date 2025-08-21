#pragma once

#include <stdint.h>
#include "gpio.h"
#include "dma.h"



typedef struct
{
    uint32_t SR;
    uint32_t DR;
    uint32_t BRR;
    uint32_t CR1;
    uint32_t CR2;
    uint32_t CR3;
    uint32_t GTPR;
} USART_Type;



// SR
#define USART_SR_PE (0x1 << 0)
#define USART_SR_FE (0x1 << 1)
#define USART_SR_NE (0x1 << 2)
#define USART_SR_ORE (0x1 << 3)
#define USART_SR_IDLE (0x1 << 4)
#define USART_SR_RXNE (0x1 << 5)
#define USART_SR_TC (0x1 << 6)
#define USART_SR_TXE (0x1 << 7)
#define USART_SR_LBD (0x1 << 8)
#define USART_SR_CTS (0x1 << 9)

// BRR
#define USART_BRR(mantissa, fraction) ((mantissa << 4) | (fraction << 0))


// CR1
#define USART_CR1_UE (1U << 13)
#define USART_CR1_M (1U << 12)   // (1 = 9 бит)
#define USART_CR1_WAKE (1U << 11)   
#define USART_CR1_PCE (1U << 10) // паритет включен
#define USART_CR1_PS (1U << 9)   // 1 = нечетный
#define USART_CR1_PEIE (1U << 8)
#define USART_CR1_TXEIE (1U << 7)
#define USART_CR1_TCIE (1U << 6)
#define USART_CR1_RXNEIE (1U << 5)
#define USART_CR1_IDLEIE (1U << 4)
#define USART_CR1_TE (1U << 3)
#define USART_CR1_RE (1U << 2)
#define USART_CR1_RWU (1U << 1)
#define USART_CR1_SBK (1U << 0)

// CR2
#define USART_CR2_LINEN (1U << 14)
#define USART_CR2_STOP(stop_bits) (stop_bits << 12)
#define USART_CR2_CLKEN (1U << 11)
#define USART_CR2_CPOL (1U << 10)
#define USART_CR2_CPHA (1U << 9)
#define USART_CR2_LBCL (1U << 8)
#define USART_CR2_LBDIE (1U << 6)
#define USART_CR2_LBDL (1U << 5) //(1 = 11 bit break detection)
#define USART_CR2_ADD(address) ((address & 0xf) << 0)

typedef enum {
    USART_STOP_1BIT   = 0x0,
    USART_STOP_0_5BIT = 0x1,
    USART_STOP_2BIT   = 0x2,
    USART_STOP_1_5BIT = 0x3
} USART_StopBits;

// CR3
#define USART_CR3_CTSIE (1U << 10)
#define USART_CR3_CTSE (1U << 9)  
#define USART_CR3_RTSE (1U << 8) 
#define USART_CR3_DMAT (1U << 7)
#define USART_CR3_DMAR (1U << 6)
#define USART_CR3_SCEN (1U << 5)
#define USART_CR3_NACK (1U << 4)
#define USART_CR3_HDSEL (1U << 3)
#define USART_CR3_IRLP (1U << 2)
#define USART_CR3_IREN (1U << 1)
#define USART_CR3_EIE (0x1 << 0)


typedef enum
{
    UART_BAUDRATE_1200 = 1200,
    UART_BAUDRATE_2400 = 2400,
    UART_BAUDRATE_4800 = 4800,
    UART_BAUDRATE_9600 = 9600,
    UART_BAUDRATE_19200 = 19200,
    UART_BAUDRATE_38400 = 38400,
    UART_BAUDRATE_57600 = 57600,
    UART_BAUDRATE_115200 = 115200,
} UART_BaudRate_t;

typedef enum
{
    UART_MODE_DISABLE = 0,
    UART_MODE_POLLING,
    UART_MODE_DMA
} UART_Mode;

typedef struct
{
    USART_Type *usart;
    UART_BaudRate_t baud_rate;
    UART_Mode tx_mode;
    UART_Mode rx_mode;
    DMA_Config dma_tx;         // DMA конфигурация для передачи (опционально)
    DMA_Config dma_rx;         // DMA конфигурация для приема (опционально)
    GPIO_PinConfig_t *tx_port; // пин TX
    GPIO_PinConfig_t *rx_port; // пин RX
} UART_Config_t;

typedef struct
{
    USART_Type *usart;
    UART_Mode tx_mode;
    UART_Mode rx_mode;
    DMA_Config dma_tx; // DMA конфигурация для передачи
    DMA_Config dma_rx; // DMA конфигурация для приема
} UART_HandleTypeDef;

void setup_uart(UART_Config_t *cfg, uint32_t usart_clk);
void send_data_usart(uint8_t *message, uint16_t length);
void send_byte_usart(uint8_t byte);

