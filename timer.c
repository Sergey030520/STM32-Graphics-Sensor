#include "timer.h"
#include "led.h"
#include "configuration.h"

#define TIM4_CH1_Port 6

void delay_timer(uint16_t ms)
{
    GP_Timer_Type1 *timer = TIM2_Reg;
    timer->CR1 = 0;
    // timer->DIER = TIMER_DIER_UIE;
    timer->PSC = 36000 - 1;
    timer->CNT = 0; // 1 msec
    timer->ARR = ms - 1;
    timer->CR1 |= TIMER_CR1_CEN; // TIMER_CR1_ARPE |

    while (!(timer->SR & TIMER_SR_UIF))
        ;

    timer->SR &= ~TIMER_SR_UIF;
    timer->CR1 &= ~TIMER_CR1_CEN;
}
