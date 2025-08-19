#include <stdint.h>

#pragma once

typedef struct
{
    uint32_t CRL;
    uint32_t CRH;
    uint32_t IDR;
    uint32_t ODR;
    uint32_t BSRR;
    uint32_t BRR;
    uint32_t LCKR;
} GPIO_Type;

typedef enum
{
    GPIO_INPUT_ANALOG = 0x0,
    GPIO_INPUT_FLOATING = 0x1,
    GPIO_INPUT_PUPD = 0x2,
} GPIO_InputConfig;

typedef enum
{
    GPIO_OUTPUT_GP_PP = 0x0, // Push pull
    GPIO_OUTPUT_GP_OD = 0x1, // Open drain
    GPIO_OUTPUT_AF_PP = 0x2, // Push pull
    GPIO_OUTPUT_AF_OD = 0x3, // Open drain
} GPIO_OutputConfig;

typedef enum
{
    GPIO_MODE_INPUT = 0x0,   // Input mode
    GPIO_OUTPUT_10MHz = 0x1, // Output mode
    GPIO_OUTPUT_2MHz = 0x2,  // Output mode
    GPIO_OUTPUT_50MHz = 0x3  // Output mode
} GPIO_MODE_Speed;


#define GPIO_MASK_CRx(port) ((uint32_t)0b1111 << ((port % 8) * 4))
#define GPIO_CONFIG_PORT(port, mode, config) ((uint32_t)((config << 2) | mode) << ((port % 8) * 4))
#define GPIO_SET_BS(pin)   ((uint32_t)(1U << (pin)))
#define GPIO_SET_BR(pin)   ((uint32_t)(1U << ((pin) + 16U)))

// ODR

typedef struct
{
    uint32_t ECVR;
    uint32_t MAPR;
    uint32_t EXTICR1;
    uint32_t EXTICR2;
    uint32_t EXTICR3;
    uint32_t EXTICR4;
    uint32_t MAPR2;
} AFIO_Type;

#define AFIO_USART1_NO_REMAP (0x0 << 2)
#define AFIO_USART1_REMAP (0x1 << 2)

typedef struct
{
    GPIO_Type *gpiox;
    uint16_t pin;
    GPIO_MODE_Speed speed;
    uint8_t pin_mode;
    uint8_t af_remap;
} GPIO_PinConfig_t;



void reset_gpio_port(GPIO_Type *gpiox, uint16_t pin);
void set_gpio_conf(GPIO_PinConfig_t *config);
void set_pin_gpio(GPIO_Type *port, uint16_t pin);
void reset_pin_gpio(GPIO_Type *port, uint16_t pin);
uint8_t get_gpio_value(GPIO_Type *gpiox, uint16_t port);