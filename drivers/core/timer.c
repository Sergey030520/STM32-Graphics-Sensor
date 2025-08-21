#include "timer.h"
#include "memory_map.h"
#include "rcc.h"

void init_timer()
{
    enable_and_reset_rcc(RCC_BUS_APB1, RCC_APB1ENR_TIM2EN);
    RCC_Frequencies rcc_clocks = {0};
    RCC_BusConfig rcc_config = {0};
    get_clock_frequencies(&rcc_clocks);
    get_rcc_clock_dividers(&rcc_config);
    GP_Timer_Type *timer = (GP_Timer_Type *)TIM2_REG;
    timer->CR1 = 0;

    uint32_t timer_clk = rcc_clocks.APB1_Freq;
    if (rcc_config.apb1_prescaler > 1)
        timer_clk *= 2;
    timer->PSC = ((timer_clk + 500) / 1000) - 1;
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

void set_pwm_timer(GP_Timer_Type *timer, uint8_t volume)
{
    timer->CCR1 = volume;
}



void init_pwm_timer(PWM_Config_t *cfg)
{
    GP_Timer_Type *timer = cfg->timer;

    timer->CR1 = 0;
    timer->CNT = 0;

    timer->PSC = cfg->prescaler;
    timer->ARR = cfg->period;

    switch (cfg->channel)
    {
    case TIM_CHANNEL1:
        timer->CCMR1 &= ~(0x7 << 4); 
        timer->CCMR1 |= TIMER_CCMRx_OC1M_PWM << 4;
        timer->CCER &= ~0x1;
        timer->CCER |= 0x1; 
        timer->CCR1 = (cfg->period + 1) * cfg->duty_cycle / 100;
        break;

    case TIM_CHANNEL2:
        timer->CCMR1 &= ~(0x7 << 12); 
        timer->CCMR1 |= TIMER_CCMRx_OC1M_PWM << 12;
        timer->CCER &= ~0x10; 
        timer->CCER |= 0x10;  
        timer->CCR2 = (cfg->period + 1) * cfg->duty_cycle / 100;
        break;

    case TIM_CHANNEL3:
        timer->CCMR2 &= ~(0x7 << 4); 
        timer->CCMR2 |= TIMER_CCMRx_OC1M_PWM << 4;
        timer->CCER &= ~0x100; 
        timer->CCER |= 0x100;  
        timer->CCR3 = (cfg->period + 1) * cfg->duty_cycle / 100;
        break;

    case TIM_CHANNEL4:
        timer->CCMR2 &= ~(0x7 << 12); 
        timer->CCMR2 |= TIMER_CCMRx_OC1M_PWM << 12;
        timer->CCER &= ~0x1000; 
        timer->CCER |= 0x1000;  
        timer->CCR4 = (cfg->period + 1) * cfg->duty_cycle / 100;
        break;

    default:
        break;
    }

    timer->CCMR1 |= TIMER_CCMRx_OC1M_PWM << 4;
    timer->CCER |= TIMER_CCER_CCx(cfg->channel, 0);

    timer->CR1 |= TIMER_CR1_ARPE | TIMER_CR1_CEN;
    set_pwm_timer(timer, cfg->duty_cycle);
}
