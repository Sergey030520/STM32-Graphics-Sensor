#include "timer.h"
#include "memory_map.h"
#include "rcc.h"


void init_timer()
{
    RCC_Frequencies rcc_clocks = {0};
    RCC_BusConfig rcc_config = {0};
    get_clock_frequencies(&rcc_clocks);
    get_rcc_clock_dividers(&rcc_config);
    GP_Timer_Type *timer = (GP_Timer_Type *)TIM2_REG;
    timer->CR1 = 0;

    uint32_t timer_clk = rcc_clocks.APB1_Freq;
    if (rcc_config.apb1_prescaler > 1)
        timer_clk *= 2;
    timer->PSC = (timer_clk / 1000) - 1;
    timer->CR1 |= TIMER_CR1_ARPE;
    timer->EGR = TIMER_EGR_UG;
}

void delay_timer(uint32_t ms)
{
    GP_Timer_Type *timer = (GP_Timer_Type *)TIM2_REG;
    timer->CR1 &= ~TIMER_CR1_CEN;
    timer->ARR = (ms ? ms - 1 : 0);
    timer->CNT = 0;
    timer->EGR = TIMER_EGR_UG;
    timer->SR = 0;

    timer->CR1 |= TIMER_CR1_CEN;

    while (!(timer->SR & TIMER_SR_UIF))
        ;

    timer->CR1 &= ~TIMER_CR1_CEN;
    timer->SR = 0;
}
