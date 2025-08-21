#pragma once

#include <stdint.h>

// NVIC

typedef struct
{
    uint32_t ICTR; // Interrupt Controller Type Register
    uint32_t RESERV1[62];
    uint32_t ISERx[8]; // Interrupt Set-Enable Registers
    uint32_t RESERV2[24];
    uint32_t ICERx[8]; // Interrupt Clear-Enable Registers
    uint32_t RESERV3[24];
    uint32_t ISPRx[8]; // Interrupt Set-Pending Registers
    uint32_t RESERV4[24];
    uint32_t ICPRx[8]; // Interrupt Clear-Pending Registers
    uint32_t RESERV5[24];
    uint32_t IABRx[8]; // Interrupt Active Bit Register
    uint32_t RESERV6[56];
    uint32_t IPRx[60]; // Interrupt Priority Register
} NVIC_Type;

typedef enum
{
    FLASH_IRQ = 4,
    RCC_IRQ = 5,
    DMA1_Channel1_IRQ = 11,
    DMA1_Channel2_IRQ = 12,
    DMA1_Channel3_IRQ = 13,
    DMA1_Channel4_IRQ = 14,
    DMA1_Channel5_IRQ = 15,
    DMA1_Channel6_IRQ = 16,
    DMA1_Channel7_IRQ = 17,
    ADC1_2_IRQ = 18,
    TIM2_IRQ = 28,
    SPI1_IRQ = 35,
    SPI2_IRQ = 36,
    USART1_IRQ = 37,
    USART2_IRQ = 38,
    USART3_IRQ = 39,
    ADC3_IRQ = 47,
}STM8_IRQn;

#define NVIC_DMA1_CHANNEL3 (0x1 << 13)
#define NVIC_DMA1_CHANNEL4 (0x1 << 14)
#define NVIC_DMA1_CHANNEL5 (0x1 << 15)
#define NVIC_SPI1 (0x1 << 3)
#define NVIC_SPI2 (0x1 << 4)
#define NVIC_USART1 (0x1 << 5)
#define NVIC_SPI3 (0x1 << 19)

void init_irq();
void enable_irq_nvic(uint8_t irqn, uint8_t priority);
void disable_irq_nvic(uint8_t irqn);