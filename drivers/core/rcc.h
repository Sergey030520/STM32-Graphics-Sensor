#include <stdint.h>

#pragma once

typedef enum
{
    FREQ_8MHZ = 8000000,
    FREQ_20MHZ = 20000000,
    FREQ_24MHZ = 24000000,
    FREQ_48MHZ = 48000000,
    FREQ_72MHZ = 72000000,
} Frequency;

#define RCC_HSI_FREQ FREQ_8MHZ
#define RCC_HSE_FREQ FREQ_8MHZ

typedef struct
{
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
} RCC_Type;

#define RCC_CR_PLLRDY (0x1 << 25)
#define RCC_CR_PLLON (0x1 << 24)
#define RCC_PLLOFF (0x0 << 24)
#define RCC_CR_CSSON (0x1 << 19)
#define RCC_CR_HSEON (0x1 << 16)
#define RCC_CR_HSERDY (0x1 << 17)

#define RCC_CR_HSION (0x1 << 16)
#define RCC_CR_HSIRDY (0x1 << 17)

// CFGR

#define RCC_CFGR_MCO_HSE (0b110 << 24)
#define RCC_CFGR_MCO_HSI (0b101 << 24)
#define RCC_CFGR_MCO_PLL (0b111 << 24)

#define RCC_CFGR_SW(source) (source << 0)
#define RCC_CFGR_SW_MASK RCC_CFGR_SW(0x3)

#define RCC_CFGR_SWS(source) (source << 2)
#define RCC_CFGR_SWS_MASK RCC_CFGR_SWS(0x3)


#define RCC_CFGR_PLLXTPRE (0x1 << 17)
#define RCC_CFGR_PLLXTPRE_MASK RCC_CFGR_PLLXTPRE

#define RCC_CFGR_PLLSRC(source) (source << 16)

#define RCC_CFGR_PLLMUL(val) ((uint32_t)val << 18U)
#define RCC_CFGR_PLLMUL_MASK ((uint32_t)0xF << 18U)

typedef enum
{
    RCC_PLL_MUL2 = 0x0,
    RCC_PLL_MUL3,
    RCC_PLL_MUL4,
    RCC_PLL_MUL5,
    RCC_PLL_MUL6,
    RCC_PLL_MUL7,
    RCC_PLL_MUL8,
    RCC_PLL_MUL9,
    RCC_PLL_MUL10,
    RCC_PLL_MUL11,
    RCC_PLL_MUL12,
    RCC_PLL_MUL13,
    RCC_PLL_MUL14,
    RCC_PLL_MUL15,
    RCC_PLL_MUL16
} RCC_PLLMul;


#define RCC_CFGR_PLLSRC_MASK RCC_CFGR_PLLSRC(1)

#define RCC_CFGR_ADCPRE(value) (value << 14)
#define RCC_CFGR_ADCPRE_MASK (0x3 << 14)

typedef enum
{
    RCC_ADCPRE_DIV2 = 0x0,
    RCC_ADCPRE_DIV4 = 0x1,
    RCC_ADCPRE_DIV6 = 0x2,
    RCC_ADCPRE_DIV8 = 0x3,
} RCC_ADC_Presc;

#define RCC_CFGR_PPRE2(value) (value << 11)
#define RCC_CFGR_PPRE2_MASK (0x7 << 11)

#define RCC_CFGR_PPRE1(value) (value << 8)
#define RCC_CFGR_PPRE1_MASK (0x7 << 8)

typedef enum
{
    RCC_NOT_DIV = 0x0,
    RCC_APB_DIV2 = 0x4,
    RCC_APB_DIV4 = 0x5,
    RCC_APB_DIV8 = 0x6,
    RCC_APB_DIV16 = 0x7,
} RCC_APB_Presc;

#define RCC_CFGR_HPRE(value) (value << 4)
#define RCC_CFGR_HPRE_MASK (0xF << 4)

#define RCC_CFGR_USBPRE (0x1 << 22)

typedef enum
{
    RCC_HPRE_DIV2 = 0x8,
    RCC_HPRE_DIV4 = 0x9,
    RCC_HPRE_DIV8 = 0xA,
    RCC_HPRE_DIV16 = 0xB,
    RCC_HPRE_DIV64 = 0xC,
    RCC_HPRE_DIV128 = 0xD,
    RCC_HPRE_DIV256 = 0xE,
    RCC_HPRE_DIV512 = 0xF
} RCC_AHB_Presc;

typedef enum {
    USB_PRESCALER_DIV1 = 0,    
    USB_PRESCALER_DIV1_5 = 1   
} USB_Prescaler_t;

// APB2ENR
#define RCC_APB2ENR_TIM11EN (0x1 << 21)
#define RCC_APB2ENR_TIM10EN (0x1 << 20)
#define RCC_APB2ENR_TIM9EN (0x1 << 19)
#define RCC_APB2ENR_ADC3EN (0x1 << 15)
#define RCC_APB2ENR_USART1EN (0x1 << 14)
#define RCC_APB2ENR_TIM8EN (0x1 << 13)
#define RCC_APB2ENR_SPI1EN (0x1 << 12)
#define RCC_APB2ENR_TIM1EN (0x1 << 11)
#define RCC_APB2ENR_ADC2EN (0x1 << 10)
#define RCC_APB2ENR_ADC1EN (0x1 << 9)
#define RCC_APB2ENR_IOPGEN (0x1 << 8)
#define RCC_APB2ENR_IOPFEN (0x1 << 7)
#define RCC_APB2ENR_IOPEEN (0x1 << 6)
#define RCC_APB2ENR_IOPDEN (0x1 << 5)
#define RCC_APB2ENR_IOPCEN (0x1 << 4)
#define RCC_APB2ENR_IOPBEN (0x1 << 3)
#define RCC_APB2ENR_IOPAEN (0x1 << 2)
#define RCC_APB2ENR_AFIOEN (0x1 << 0)

// APB1ENR

#define RCC_APB1ENR_DACEN (0x1 << 29)
#define RCC_APB1ENR_PWREN (0x1 << 28)
#define RCC_APB1ENR_BKPEN (0x1 << 27)
#define RCC_APB1ENR_CANEN (0x1 << 25)

#define RCC_APB1ENR_USBEN (0x1 << 23)
#define RCC_APB1ENR_I2C2EN (0x1 << 22)
#define RCC_APB1ENR_I2C1EN (0x1 << 21)
#define RCC_APB1ENR_UART5EN (0x1 << 20)
#define RCC_APB1ENR_UART4EN (0x1 << 19)
#define RCC_APB1ENR_USART3EN (0x1 << 18)
#define RCC_APB1ENR_USART2EN (0x1 << 17)
#define RCC_APB1ENR_SPI3EN (0x1 << 15)
#define RCC_APB1ENR_SPI2EN (0x1 << 14)
#define RCC_APB1ENR_WWDGEN (0x1 << 11)
#define RCC_APB1ENR_TIM14 (0x1 << 8)
#define RCC_APB1ENR_TIM13 (0x1 << 7)
#define RCC_APB1ENR_TIM12 (0x1 << 6)
#define RCC_APB1ENR_TIM7 (0x1 << 5)
#define RCC_APB1ENR_TIM6 (0x1 << 4)
#define RCC_APB1ENR_TIM5 (0x1 << 3)
#define RCC_APB1ENR_TIM4 (0x1 << 2)
#define RCC_APB1ENR_TIM3 (0x1 << 1)
#define RCC_APB1ENR_TIM2 (0x1 << 0)

// AHBENR

#define RCC_AHBENR_SDIOEN (0x1 << 10)
#define RCC_AHBENR_FSMCEN (0x1 << 8)
#define RCC_AHBENR_CRCEN (0x1 << 6)
#define RCC_AHBENR_FLITFEN (0x1 << 4)
#define RCC_AHBENR_SRAMEN (0x1 << 2)
#define RCC_AHBENR_DMA2EN (0x1 << 1)
#define RCC_AHBENR_DMA1EN (0x1 << 0)

typedef enum
{
    RCC_HSI = 0x0,
    RCC_HSE = 0x1,
    RCC_PLL = 0x2
} RCC_ClockSource;

typedef struct
{
    uint8_t mul_factor;
    uint8_t source;
    uint8_t hse_divider;
} RCC_PLLConfig;

typedef struct
{
    uint8_t ahb_prescaler;
    uint8_t apb1_prescaler;
    uint8_t apb2_prescaler;
    uint8_t adc_prescaler;
    uint8_t usb_prescaler;
} RCC_BusConfig;

typedef struct
{
    uint32_t APB1;
    uint32_t APB2;
    uint32_t AHB;
} RCC_PeripheralClockConfig;

typedef struct
{
    RCC_ClockSource source;
    RCC_PLLConfig *pll_config;
    RCC_BusConfig *bus_config;
} RCC_SystemConfig;

typedef struct
{
    uint32_t sysclk;
    uint32_t hclk;
    uint32_t pclk1;
    uint32_t pclk2;
    uint32_t adcclk;
    uint32_t usbclk;
} RCC_Clocks;

typedef struct
{
    uint32_t SYSCLK_Freq;
    uint32_t HCLK_Freq;
    uint32_t APB1_Freq;
    uint32_t APB2_Freq;
    uint32_t ADC_Freq;
    uint32_t USB_Freq;
} RCC_Frequencies;

typedef enum
{
    RCC_BUS_AHB,
    RCC_BUS_APB1,
    RCC_BUS_APB2
} RCC_Bus;

int setup_system_config_rcc(RCC_SystemConfig *config);
void enable_gpio_clock_rcc(RCC_PeripheralClockConfig *config);
void get_clock_frequencies(RCC_Frequencies *freq);
void enable_and_reset_rcc(RCC_Bus bus, uint32_t periph_mask);
void get_rcc_clock_dividers(RCC_BusConfig *config);
RCC_ClockSource get_sysclk_source();