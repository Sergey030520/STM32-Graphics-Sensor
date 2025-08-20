#include "led.h"
#include "board_pins.h"
#include "timer.h"
#include "rcc.h"

GPIO_Type *LED = LED_PORT;

void init_led()
{
    enable_and_reset_rcc(RCC_BUS_APB2, RCC_APB2ENR_IOPCEN);
    GPIO_PinConfig_t pin_config = {
        .gpiox = LED,
        .pin = LED_PIN,
        .speed = GPIO_OUTPUT_2MHz,
        .pin_mode = GPIO_OUTPUT_GP_PP,
        .af_remap = 0,
    };
    set_gpio_conf(&pin_config);
    set_pin_gpio(LED, LED_PIN);
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
        reset_pin_gpio(LED, LED_PIN); // включить LED
    }
    else
    {
        set_pin_gpio(LED, LED_PIN); // выключить LED
    }
}
