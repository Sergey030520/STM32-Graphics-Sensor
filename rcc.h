#include <stdint.h>

#pragma once

#define RCC_REG 0x40021000U

typedef struct{
    uint32_t CR;
    uint32_t CFGR;
    uint32_t CIR;
    uint32_t APB2RSTR;
    uint32_t APB1RSTR;
    uint32_t AHBENR;
    uint32_t APB2ENR;
    uint32_t APB1ENR;
    uint32_t BDCR;
    uint32_t CSR;
}RCC_Type;


#define RCC_PLLRDY (0x1 << 25)
#define RCC_PLLON (0x1 << 24)
#define RCC_PLLOFF (0x0 << 24)
#define RCC_CSSON (0x1 << 19)
#define RCC_HSEON (0x1 << 16)
#define RCC_HSERDY (0x1 << 17)

#define RCC_HSION (0x1 << 16)
#define RCC_HSIRDY (0x1 << 17)


#define RCC_MCO_HSE (0b110 << 24)
#define RCC_MCO_HSI (0b101 << 24)
#define RCC_MCO_PLL (0b111 << 24)

#define RCC_PLLSRC_HSE (0b1 << 16)
#define RCC_SWS_HSE (0b01 << 2)
#define RCC_SW_HSE (0b01 << 0) 
#define RCC_SWS_PLL (0b10 << 2)
#define RCC_SW_PLL (0b10 << 0) 
#define RCC_SWS_HSI (0b00 << 2)
#define RCC_SW_HSI (0b00 << 0) 

#define RCC_PLLXTRE_DIV2 (0b1 << 17)
#define RCC_PLLXTRE_NOT_DIV (0b0 << 17)

#define RCC_PPRE1_DIV2 (0b100 << 8)

#define RCC_PLLSRC_HSI (0b0 << 16)
#define RCC_PLLSRC_HSE (0b1 << 16) 
#define RCC_PLLMUL_9 (0b0111 << 18)
#define RCC_PLLMUL_8 (0b0110 << 18)
#define RCC_ADCPRE_6 (0b10 << 14)



// APB2ENR
#define RCC_TIM11EN (0x1 << 21)
#define RCC_TIM10EN (0x1 << 20)
#define RCC_TIM9EN (0x1 << 19)
#define RCC_ADC3EN (0x1 << 15)
#define RCC_USART1EN (0x1 << 14)
#define RCC_TIM8EN (0x1 << 13)
#define RCC_SPI1EN (0x1 << 12)
#define RCC_TIM1EN (0x1 << 11)
#define RCC_ADC2EN (0x1 << 10)
#define RCC_ADC1EN (0x1 << 9)
#define RCC_IOPGEN (0x1 << 8)
#define RCC_IOPFEN (0x1 << 7)
#define RCC_IOPEEN (0x1 << 6)
#define RCC_IOPDEN (0x1 << 5)
#define RCC_IOPCEN (0b1 << 4)
#define RCC_IOPBEN (0x1 << 3)
#define RCC_IOPAEN (0x1 << 2)
#define RCC_AFIOEN (0x1 << 0)


// APB1ENR

#define RCC_DACEN (0x1 << 29)
#define RCC_PWREN (0x1 << 28)
#define RCC_BKPEN (0x1 << 27)
#define RCC_CANEN (0x1 << 25)

#define RCC_USBEN (0x1 << 23)
#define RCC_I2C2EN (0x1 << 22)
#define RCC_I2C1EN (0b1 << 21)
#define RCC_UART5EN (0x1 << 20)
#define RCC_UART4EN (0x1 << 19)
#define RCC_USART3EN (0x1 << 18)
#define RCC_USART2EN (0x1 << 17)
#define RCC_SPI3EN (0x1 << 15)
#define RCC_SPI2EN (0x1 << 14)
#define RCC_WWDGEN (0x1 << 11)
#define RCC_TIM14 (0x1 << 8)
#define RCC_TIM13 (0x1 << 7)
#define RCC_TIM12 (0x1 << 6)
#define RCC_TIM7 (0x1 << 5)
#define RCC_TIM6 (0x1 << 4)
#define RCC_TIM5 (0x1 << 3)
#define RCC_TIM4 (0x1 << 2)
#define RCC_TIM3 (0x1 << 1)
#define RCC_TIM2 (0x1 << 0)


// AHBENR

#define RCC_SDIOEN (0x1 << 10)
#define RCC_FSMCEN (0x1 << 8)
#define RCC_CRCEN (0x1 << 6)
#define RCC_FLITFEN (0x1 << 4)
#define RCC_SRAMEN (0x1 << 2)
#define RCC_DMA2EN (0x1 << 1)
#define RCC_DMA1EN (0x1 << 0)