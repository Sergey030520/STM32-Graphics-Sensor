#include "rcc.h"
#include "flash.h"
#include "memory_map.h"
#include <stdlib.h>
#include "led.h"
#include "utils.h"

RCC_Type *RCC = RCC_REG;

static inline uint32_t decode_pllp(uint8_t pllp_code);
void disable_all_GPIO_clocks_rcc();
void set_bus_dividers_rcc(RCC_BusConfig *bus_conf);
int configure_pll_rcc(RCC_PLLConfig *config);
int select_sys_clock(RCC_ClockSource src);
int switch_to_hsi_rcc();
int switch_to_hse_rcc();
int switch_to_pll_rcc(RCC_PLLConfig *pll_config);

void disable_all_GPIO_clocks_rcc()
{
    // AHBx
    RCC->AHBENR = 0;
    // APBx
    RCC->APB1ENR = 0;
    RCC->APB2ENR = 0;
}

int setup_system_config_rcc(RCC_SystemConfig *config)
{
    int status = 0;

    if (config == NULL)
    {
        return -1;
    }

    disable_all_GPIO_clocks_rcc();

    RCC->CR |= RCC_CR_CSSON;

    if (config->source == RCC_HSE)
    {
        status = switch_to_hse_rcc();
    }
    else if (config->source == RCC_HSI)
    {
        status = switch_to_hsi_rcc();
    }
    else if (config->source == RCC_PLL)
    {
        status = switch_to_pll_rcc(config->pll_config);
    }
    else
    {
        return -3;
    }
    if (status != 0)
        return status;

    if (config->bus_config != NULL)
    {
        set_bus_dividers_rcc(config->bus_config);
    }
    return 0;
}

int configure_pll_rcc(RCC_PLLConfig *config)
{
    if (config == NULL)
    {
        return -1;
    }
    if (RCC->CR & RCC_CR_PLLON)
    {
        return -2;
    }
    if (config->source != RCC_HSE && config->source != RCC_HSI)
    {
        return -3;
    }

    // Сбрасываем старые значения PLL
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC_MASK | RCC_CFGR_PLLMUL_MASK | RCC_CFGR_PLLXTPRE_MASK);

    RCC->CFGR |= RCC_CFGR_PLLSRC(config->source);

    return 0;
}

void set_flash_latency(uint32_t hclk_freq)
{
    FLASH_Type *Flash = (FLASH_Type *)FLASH_REG;
    Flash->ACR &= ~FLASH_ACR_LATENCY_MASK;
    if (hclk_freq > 0 && hclk_freq <= FREQ_24MHZ)
    {
        Flash->ACR |= LATENCY_ZERO;
    }
    else if (hclk_freq > FREQ_24MHZ && hclk_freq <= FREQ_48MHZ)
    {
        Flash->ACR |= LATENCY_ONE;
    }
    else if (hclk_freq > FREQ_48MHZ && hclk_freq <= FREQ_72MHZ)
    {
        Flash->ACR |= LATENCY_TWO;
    }
    else
    {
        return -1;
    }
}

void set_bus_dividers_rcc(RCC_BusConfig *bus_conf)
{
    if (bus_conf == NULL)
    {
        return;
    }
    RCC->CFGR &= ~(RCC_CFGR_PPRE1_MASK | RCC_CFGR_PPRE2_MASK | RCC_CFGR_ADCPRE_MASK | RCC_CFGR_HPRE_MASK);
    RCC->CFGR |= RCC_CFGR_ADCPRE(bus_conf->adc_prescaler);
    RCC->CFGR |= RCC_CFGR_PPRE1(bus_conf->apb1_prescaler);
    RCC->CFGR |= RCC_CFGR_PPRE2(bus_conf->apb2_prescaler);
    RCC->CFGR |= RCC_CFGR_HPRE(bus_conf->ahb_prescaler);
}

int select_sys_clock(RCC_ClockSource src)
{
    if (src == RCC_HSI)
    {
        if (!(RCC->CR & RCC_CR_HSIRDY))
            return -1;
    }
    else if (src == RCC_HSE)
    {
        if (!(RCC->CR & RCC_CR_HSERDY))
            return -2;
    }
    else if (src == RCC_PLL)
    {
        if (!(RCC->CR & RCC_CR_PLLRDY))
            return -3;
    }
    else
    {
        return -4;
    }

    RCC->CFGR &= ~RCC_CFGR_SW_MASK;
    RCC->CFGR |= RCC_CFGR_SW(src);
    int counter = 1000000;
    while ((RCC->CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS(src))
    {
        if (--counter == 0)
            return -5;
    }
    return 0;
}

int switch_to_hsi_rcc()
{
    if (!(RCC->CR & RCC_CR_HSION))
    {
        RCC->CR |= RCC_CR_HSION;
        if (wait_for_flags(&RCC->CR, RCC_CR_HSIRDY, 100000000) != 0)
        {
            return -1;
        }
    }

    if (select_sys_clock(RCC_HSI) != 0)
        return -2;

    if (RCC->CR & RCC_CR_HSEON)
    {
        RCC->CR &= ~RCC_CR_HSEON;
    }
    if (RCC->CR & RCC_CR_PLLON)
    {
        RCC->CR &= ~RCC_CR_PLLON;
    }
    return 0;
}

int switch_to_hse_rcc()
{
    if (!(RCC->CR & RCC_CR_HSEON))
    {
        RCC->CR |= RCC_CR_HSEON;
        if (wait_for_flags(&RCC->CR, RCC_CR_HSERDY, 100000000) != 0)
        {
            return -1;
        }
    }

    if (select_sys_clock(RCC_HSE) != 0)
        return -2;

    if (RCC->CR & RCC_CR_PLLON)
    {
        RCC->CR &= ~RCC_CR_PLLON;
    }
    if (RCC->CR & RCC_CR_HSION)
    {
        RCC->CR &= ~RCC_CR_HSION;
    }
    return 0;
}

int switch_to_pll_rcc(RCC_PLLConfig *pll_config)
{
    if (!pll_config)
        return -1;

    const uint32_t TIMEOUT = 1000000;

    if (pll_config->source == RCC_HSE)
    {
        if (!(RCC->CR & RCC_CR_HSEON))
        {
            RCC->CR |= RCC_CR_HSEON;
            if (wait_for_flags(&RCC->CR, RCC_CR_HSERDY, TIMEOUT) != 0)
                return -2;
        }
    }
    else
    {
        if (!(RCC->CR & RCC_CR_HSION))
        {
            RCC->CR |= RCC_CR_HSION;
            if (wait_for_flags(&RCC->CR, RCC_CR_HSIRDY, TIMEOUT) != 0)
                return -3;
        }
    }

    uint32_t input_freq = (pll_config->source == RCC_HSE)
                              ? (pll_config->hse_divider ? RCC_HSE_FREQ / 2 : RCC_HSE_FREQ)
                              : RCC_HSI_FREQ / 2;

    uint32_t target_sysclk = input_freq * pll_config->mul_factor;

    RCC_Frequencies current_freq;
    get_clock_frequencies(&current_freq);

    if (target_sysclk > current_freq.SYSCLK_Freq)
        set_flash_latency(target_sysclk);

    if (RCC->CR & RCC_CR_PLLON)
    {
        RCC->CR &= ~RCC_CR_PLLON;
        uint32_t timeout = TIMEOUT;
        while (RCC->CR & RCC_CR_PLLRDY)
        {
            if (--timeout == 0)
                return -4;
        }
    }

    if (configure_pll_rcc(pll_config) != 0)
        return -5;

    RCC->CR |= RCC_CR_PLLON;
    if (wait_for_flags(&RCC->CR, RCC_CR_PLLRDY, TIMEOUT) != 0)
        return -6;

    if (select_sys_clock(RCC_PLL) != 0)
        return -7;

    uint32_t timeout = TIMEOUT;
    while ((RCC->CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS(RCC_PLL))
        if (--timeout == 0)
            return -8;

    if (target_sysclk < current_freq.SYSCLK_Freq)
        set_flash_latency(target_sysclk);

    return 0;
}

void enable_gpio_clock_rcc(RCC_PeripheralClockConfig *config)
{
    RCC->APB1ENR = config->APB1;
    RCC->APB2ENR = config->APB2;
    RCC->AHBENR = config->AHB;
}


void get_clock_frequencies(RCC_Frequencies *freq)
{
    if (freq == NULL)
        return;

    uint32_t clock_source = (RCC->CFGR & RCC_CFGR_SWS_MASK) >> 2;
    switch (clock_source)
    {
    case RCC_HSI:
        freq->SYSCLK_Freq = RCC_HSI_FREQ;
        break;
    case RCC_HSE:
        freq->SYSCLK_Freq = RCC_HSE_FREQ;
        break;
    case RCC_PLL:
    {
        uint32_t pll_mul = ((RCC->CFGR & RCC_CFGR_PLLMUL_MASK) >> 18) + 2;
        uint32_t pll_src = (RCC->CFGR & RCC_CFGR_PLLSRC_MASK) >> 16;
        uint32_t input_freq = 0;
        if (pll_src == RCC_HSE)
        {
            if (RCC->CFGR & RCC_CFGR_PLLXTPRE_MASK)
            {
                input_freq = RCC_HSE_FREQ / 2;
            }
            else
            {
                input_freq = RCC_HSE_FREQ;
            }
        }
        else
        {
            input_freq = RCC_HSI_FREQ / 2;
        }

        freq->SYSCLK_Freq = input_freq * pll_mul;
        break;
    }
    default:
        freq->SYSCLK_Freq = RCC_HSI_FREQ;
        break;
    }

    // Вычисляем HCLK
    uint32_t hpre = (RCC->CFGR & RCC_CFGR_HPRE_MASK) >> 4;
    if (hpre < 8)
        freq->HCLK_Freq = freq->SYSCLK_Freq;
    else
        freq->HCLK_Freq = freq->SYSCLK_Freq >> (hpre - 7);

    // APB1
    uint32_t ppre1 = (RCC->CFGR & RCC_CFGR_PPRE1_MASK) >> 8;
    if (ppre1 < 4)
        freq->APB1_Freq = freq->HCLK_Freq;
    else
        freq->APB1_Freq = freq->HCLK_Freq >> (ppre1 - 3);

    // APB2
    uint32_t ppre2 = (RCC->CFGR & RCC_CFGR_PPRE2_MASK) >> 11;
    if (ppre2 < 4)
        freq->APB2_Freq = freq->HCLK_Freq;
    else
        freq->APB2_Freq = freq->HCLK_Freq >> (ppre2 - 3);

    // ADCCLK
    uint32_t adcpre = (RCC->CFGR & RCC_CFGR_ADCPRE_MASK) >> 14;
    switch (adcpre)
    {
    case RCC_ADCPRE_DIV2:
        freq->ADC_Freq = freq->APB2_Freq / 2;
        break;
    case RCC_ADCPRE_DIV4:
        freq->ADC_Freq = freq->APB2_Freq / 4;
        break;
    case RCC_ADCPRE_DIV6:
        freq->ADC_Freq = freq->APB2_Freq / 6;
        break;
    case RCC_ADCPRE_DIV8:
        freq->ADC_Freq = freq->APB2_Freq / 8;
        break;
    default:
        freq->ADC_Freq = freq->APB2_Freq;
        break;
    }

    if (RCC->CFGR & RCC_CFGR_USBPRE) // divide by 1
        freq->USB_Freq = freq->HCLK_Freq / 1;
    else // divide by 1.5
        freq->USB_Freq = (freq->HCLK_Freq * 2) / 3;
}

void enable_and_reset_rcc(RCC_Bus bus, uint32_t periph_mask)
{
    switch (bus)
    {
    case RCC_BUS_AHB:
        RCC->AHBENR |= periph_mask;
        break;

    case RCC_BUS_APB1:
        RCC->APB1ENR |= periph_mask;
        break;

    case RCC_BUS_APB2:
        RCC->APB2ENR |= periph_mask;
        break;
    }
}

void get_rcc_clock_dividers(RCC_BusConfig *config)
{
    if (config == NULL)
        return;

    // AHB (HCLK)
    config->ahb_prescaler = (RCC->CFGR & RCC_CFGR_HPRE_MASK) >> 4;

    // APB1 (PCLK1)
    config->apb1_prescaler = (RCC->CFGR & RCC_CFGR_PPRE1_MASK) >> 8;

    // APB2 (PCLK2)
    config->apb2_prescaler = (RCC->CFGR & RCC_CFGR_PPRE2_MASK) >> 11;

    // ADC
    config->adc_prescaler = (RCC->CFGR & RCC_CFGR_ADCPRE_MASK) >> 14;

    // USB prescaler (делитель 1.5 или 1)
    config->usb_prescaler = (RCC->CFGR & RCC_CFGR_USBPRE) ? 1 : 0;
}
