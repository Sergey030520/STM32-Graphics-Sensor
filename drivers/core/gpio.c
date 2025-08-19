#include "gpio.h"
#include "led.h"
#include <stdio.h>
#include "memory_map.h"



AFIO_Type *AFIO = AFIO_REG;

void reset_gpio_port(GPIO_Type *gpiox, uint16_t pin)
{
    if(pin < 8)
        gpiox->CRL &= ~GPIO_MASK_CRx(pin);
    else
        gpiox->CRH &= ~GPIO_MASK_CRx(pin);
}

void set_gpio_conf(GPIO_PinConfig_t *config)
{
    if (config == NULL || config->gpiox == NULL) {
        return;
    }

    GPIO_Type *gpiox = config->gpiox;
    uint16_t pin = config->pin;
    uint32_t shift = (pin % 8) * 4;

    if (pin < 8) {
        gpiox->CRL &= ~(0xF << shift);
        gpiox->CRL |= ((config->speed & 0x3) | ((config->pin_mode & 0x3) << 2)) << shift;
    } else {
        gpiox->CRH &= ~(0xF << shift);
        gpiox->CRH |= ((config->speed & 0x3) | ((config->pin_mode & 0x3) << 2)) << shift;
    }

    if (config->af_remap != 0) {
        AFIO->MAPR |= config->af_remap;
    }
}


void set_pin_gpio(GPIO_Type *gpiox, uint16_t pin)
{
    gpiox->BSRR = GPIO_SET_BS(pin);
}

void reset_pin_gpio(GPIO_Type *gpiox, uint16_t pin)
{
    gpiox->BSRR = GPIO_SET_BR(pin);
}

uint8_t get_gpio_value(GPIO_Type *gpiox, uint16_t port)
{
     return (gpiox->IDR & (1 << port)) ? 1 : 0;
}