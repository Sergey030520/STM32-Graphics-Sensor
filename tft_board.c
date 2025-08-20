#include "tft_board.h"
#include "gpio.h"
#include "memory_map.h"
#include "timer.h"
#include "tft.h"
#include "spi_board.h"
#include <stdlib.h>



#define TFT_RES_Port GPIOA_REG
#define TFT_RES_Pin 1
#define TFT_DC_Port GPIOA_REG
#define TFT_DC_Pin 2
#define TFT_BLK_Port GPIOB_REG
#define TFT_BLK_Pin 6



void tft_set_dc(int state)
{
    if (state)
        set_pin_gpio(TFT_DC_Port, TFT_DC_Pin);
    else
        reset_pin_gpio(TFT_DC_Port, TFT_DC_Pin);
}

void tft_set_res(int state)
{
    if (state)
        set_pin_gpio(TFT_RES_Port, TFT_RES_Pin);
    else
        reset_pin_gpio(TFT_RES_Port, TFT_RES_Pin);
}

void tft_set_blk(int state)
{
    if (state)
        set_pin_gpio(TFT_BLK_Port, TFT_BLK_Pin);
    else
        reset_pin_gpio(TFT_BLK_Port, TFT_BLK_Pin);
}

void tft_delay_ms(uint32_t ms)
{
    delay_timer(ms);
}

void tft_set_brightness(uint8_t value)
{
    set_pwm_timer(TIM4_REG, value);
}

void tft_init_board_interface(TFT_Interface_t *tft)
{
    //  gpiob->CRL |= GPIO_CRL_CNF_MODE(BLK_Port, GPIO_MODE_SPEED_50MHz, GPIO_AF_PUSH_PULL);
    //  gpioa->CRL |= GPIO_CRL_CNF_MODE(RES_Port, GPIO_MODE_SPEED_50MHz, GPIO_OUTPUT_PUSH_PULL);
    // gpioa->CRL |= GPIO_CRL_CNF_MODE(DC_Port, GPIO_MODE_SPEED_50MHz, GPIO_OUTPUT_PUSH_PULL);

    GPIO_PinConfig_t pin_blk = {
        .gpiox = TFT_BLK_Port,
        .pin = TFT_BLK_Pin,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_AF_PP,
        .af_remap = 0,
    };
    GPIO_PinConfig_t pin_res = {
        .gpiox = TFT_RES_Port,
        .pin = TFT_RES_Pin,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_GP_PP,
        .af_remap = 0,
    };
    GPIO_PinConfig_t pin_dc = {
        .gpiox = TFT_DC_Port,
        .pin = TFT_DC_Pin,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_GP_PP,
        .af_remap = 0,
    };

    tft->set_cs = tft_set_cs;
    tft->set_dc = tft_set_dc;
    tft->set_res = tft_set_res;
    tft->set_blk = tft_set_blk;
    tft->spi_send = tft_spi_send;
    tft->spi_recv = tft_spi_recv;
    tft->delay_ms = tft_delay_ms;
    tft->set_brightness = tft_set_brightness;
}
