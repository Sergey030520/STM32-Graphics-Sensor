#include "configuration.h"
#include "timer.h"

#define LED 13

void init_led()
{
    gpioc->CRH &= ~GPIO_MASK_PORT_CRH(LED);
    gpioc->CRH |= GPIO_CRH_CNF_MODE(LED, GPIO_MODE_SPEED_50MHz,
       GPIO_OUTPUT_PUSH_PULL);
    ledOn(0);
}



void signal_error(int count_signal)
{
    for (int i = 0; i < count_signal; ++i)
    {
        ledOn(1);
        delay_timer(5000);
        ledOn(0);
        delay_timer(5000);
    }
}


void ledOn(int state)
{
    if (state)
    {
        gpioc->BSRR = (0x1 << (LED + 16));
    }
    else
    {
        gpioc->BSRR = (0x1 << LED);
    }
}
