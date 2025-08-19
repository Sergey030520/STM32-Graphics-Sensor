#include "led.h"
#include "board_pins.h"
#include "timer.h"
#include "rcc.h"



void init_led()
{
    enable_and_reset_rcc(RCC_BUS_APB2, RCC_IOPBEN);
    GPIO_PinConfig_t pin_config = {
        .gpiox = (uint32_t *)LED_PORT,
        .pin = LED_PIN,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_GP_PP,
        .af_remap = 0,
    };
    set_gpio_conf(&pin_config);
    set_pin_gpio((uint32_t *) LED_PORT, LED_PIN);
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
        set_pin_gpio((uint32_t *)LED_PORT, LED_PIN);
    }
    else
    {
        reset_pin_gpio((uint32_t *)LED_PORT, LED_PIN);
    }
}
