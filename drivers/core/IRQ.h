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