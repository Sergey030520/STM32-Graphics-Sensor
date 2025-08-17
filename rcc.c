#include "rcc.h"
#include "configuration.h"


RCC_Type *RCC = RCC_REG;
FLASH_Type *Flash = FLASH_REG;

int init_rcc()
{
    // выключаем
    RCC->APB2ENR = 0; 
    RCC->APB1ENR = 0;
    RCC->CFGR = 0;
    for (int tick = 0; tick < 1000000; ++tick)
    {
        if (RCC->CFGR & RCC_SWS_HSI)
        {
            break;
        }
    }

    // Включаем внешний генератор
    RCC->CR = RCC_HSION | RCC_HSEON | RCC_CSSON; 
    
    int state = 1;
    for (int tick = 0; tick < 1000000; ++tick)
    {
        if (RCC->CR & RCC_HSERDY)
        {
            state = 0;
            break;
        }
    }
    if (state != 0)
    {
        RCC->CR &= ~RCC_HSEON;
        return -1;
    }


    RCC->CFGR |= RCC_PLLSRC_HSE | RCC_PPRE1_DIV2 | RCC_PLLMUL_9 | RCC_ADCPRE_6;
    
    RCC->CR |= RCC_PLLON;
    state = 1;
    for (int tick = 0; tick < 1000000; ++tick)
    {
        if (RCC->CR & RCC_PLLRDY)
        {
            state = 0;
            break;
        }
    }
    if (state != 0)
    {
        RCC->CR &= ~RCC_HSEON;
        RCC->CR &= ~RCC_PLLON;
        return -2;
    }

    Flash->ACR |= FLASH_LATENCY_TWO;

    RCC->CFGR |= RCC_SW_PLL;
    state = 1;
    for (int tick = 0; tick < 1000000; ++tick)
    {
        if (RCC->CFGR & RCC_SWS_PLL)
        {
            state = 0;
            break;
        }
    }
    if (state != 0)
        return -3;

       
    RCC->APB2ENR |= RCC_IOPCEN | RCC_IOPAEN | RCC_IOPBEN | RCC_USART1EN | RCC_SPI1EN; //| RCC_AFIOEN;
    RCC->APB1ENR |= RCC_TIM2 | RCC_TIM4;//RCC_SPI2EN | RCC_SPI3EN;
    RCC->AHBENR =  RCC_FLITFEN | RCC_SRAMEN | RCC_DMA1EN; 
    return 0;
}