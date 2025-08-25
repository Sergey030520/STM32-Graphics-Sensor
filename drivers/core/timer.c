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

void set_pwm_timer(GP_Timer_Type *timer, uint8_t duty)
{
    uint32_t arr = timer->ARR;
    timer->CCR1 = (arr + 1) * duty / 100;
}

void init_pwm_timer(PWM_Config_t *cfg, uint32_t timer_clk)
{
    GP_Timer_Type *timer = cfg->timer;

    // Сброс таймера
    timer->CR1 = 0;
    timer->CNT = 0;

    
    uint32_t target_freq = cfg->pwm_freq;
    uint32_t prescaler = 0;
    uint32_t arr = 0xFFFF;

    for (prescaler = 0; prescaler <= 0xFFFF; prescaler++)
    {
        arr = timer_clk / ((prescaler + 1) * target_freq) - 1;
        if (arr <= 0xFFFF)
            break;
    }

    timer->PSC = prescaler;
    timer->ARR = arr;

    // Вычисляем CCR для скважности
    uint32_t ccr_val = (arr + 1) * cfg->duty_cycle / 100;

    // Настраиваем канал
    switch (cfg->channel)
    {
        case TIM_CHANNEL1:
            timer->CCMR1 &= ~(0x7 << 4);
            timer->CCMR1 |= (TIMER_CCMRx_OC1M_PWM << 4);
            timer->CCMR1 |= (1 << 3); // OC1PE preload enable
            timer->CCER |= 0x1;       // CC1E enable
            timer->CCR1 = ccr_val;
            break;

        case TIM_CHANNEL2:
            timer->CCMR1 &= ~(0x7 << 12);
            timer->CCMR1 |= (TIMER_CCMRx_OC1M_PWM << 12);
            timer->CCMR1 |= (1 << 11); // OC2PE preload enable
            timer->CCER |= 0x10;       // CC2E enable
            timer->CCR2 = ccr_val;
            break;

        case TIM_CHANNEL3:
            timer->CCMR2 &= ~(0x7 << 4);
            timer->CCMR2 |= (TIMER_CCMRx_OC1M_PWM << 4);
            timer->CCMR2 |= (1 << 3); // OC3PE preload enable
            timer->CCER |= 0x100;     // CC3E enable
            timer->CCR3 = ccr_val;
            break;

        case TIM_CHANNEL4:
            timer->CCMR2 &= ~(0x7 << 12);
            timer->CCMR2 |= (TIMER_CCMRx_OC1M_PWM << 12);
            timer->CCMR2 |= (1 << 11); // OC4PE preload enable
            timer->CCER |= 0x1000;     // CC4E enable
            timer->CCR4 = ccr_val;
            break;

        default:
            return;
    }

    // Генерация события обновления и включение таймера
    timer->EGR |= TIMER_EGR_UG;
    timer->CR1 |= TIMER_CR1_ARPE | TIMER_CR1_CEN;
}