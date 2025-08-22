#include <stdio.h>
#include "rcc.h"
#include "led.h"
#include "usart.h"
#include "dma.h"
#include "board_pins.h"
#include "IRQ.h"
#include "log.h"
#include "timer.h"
#include "usart_board.h"
#include "spi_board.h"
#include "tft_board.h"




int init_rcc();
int init_board();
void print_clock_frequencies();

int main()
{

    int status = init_board();
    if (status != 0)
    {
        ledOn(1);
        goto error;
    }

    LOG_INFO("Драйверы запущены\r\n");

    print_clock_frequencies();

    TFT_Interface_t tft_if = {0};

    LOG_INFO("Init tft\r\n");
    tft_init_board_interface(&tft_if);

    fill_color_display(RGB565(0, 0, 255));
    

    // draw_line(160,50, 0, 67, RGB565(255, 0, 0));

    while (1)
    {
        // Включить LED (PC13 = 0)
        ledOn(1);

        // send_byte_usart(0x11);
        delay_timer(1000);
        // for (volatile int i = 0; i < 500000; i++)
        // ;

        // Выключить LED (PC13 = 1)
        ledOn(0);
        // send_byte_usart(0x12);
        delay_timer(1000);
    }

    // init_spi_master();

    // st7789_init();

    //
    // 63 g, 31 r, 31 - b
    // uint16_t color = 0xF8;

    //
    // fill(0, 0, 239, 239, RGB565(0, 255, 0));

    // draw_line(160,50, 0, 67, RGB565(255, 0, 0));
    // draw_line(160,160, 200, 160, RGB565(0, 255, 0));

    // draw_rect(200, 200, 20, 20,  RGB565(255, 255, 0));
    // draw_rect(140, 140, 50, 50,  RGB565(0, 255, 255));
    // draw_rect(0, 0, 70, 70,  RGB565(0, 255, 255));

    // Point point1 = {60, 20}, point2 = {30, 60},
    //       point3 = {90, 60};

    // draw_triangle(point1, point2, point3, RGB565(255, 0, 0));
    // draw_circle(140, 140, 30, RGB565(0, 0, 255));
    // draw_line_bresenham(200, 200, 106, 203, RGB565(0,0,255));
    // set_brightness(90);
    // // fill();

    // delay_timer(2000);

    // set_brightness(200);

    // delay_timer(2000);

    // set_brightness(10);

    // delay_timer(2000);

    // set_brightness(30);

    while (1)
    {
        // LOG_INFO("STMf103");
        // delay_timer(2000);
    }

error:
    while (1)
        ;

    return 0;
}

int init_rcc()
{
    RCC_PLLConfig pll_config = {0};
    RCC_BusConfig bus_config = {0};
    RCC_SystemConfig system_config = {0};
    RCC_PeripheralClockConfig peripheral_conf = {0};
    int status = 0;

    pll_config.source = RCC_HSE;
    pll_config.hse_divider = 0;
    pll_config.mul_factor = RCC_PLL_MUL9;

    bus_config.adc_prescaler = RCC_ADCPRE_DIV6;
    bus_config.ahb_prescaler = RCC_NOT_DIV;
    bus_config.apb1_prescaler = RCC_APB_DIV2;
    bus_config.apb2_prescaler = RCC_NOT_DIV;
    bus_config.usb_prescaler = USB_PRESCALER_DIV1_5;

    system_config.source = RCC_PLL;
    system_config.bus_config = &bus_config;
    system_config.pll_config = &pll_config;

    status = setup_system_config_rcc(&system_config);

    if (status != 0)
    {
        pll_config.source = RCC_HSI;
        pll_config.mul_factor = RCC_PLL_MUL12;

        bus_config.ahb_prescaler = RCC_NOT_DIV;
        bus_config.adc_prescaler = RCC_ADCPRE_DIV4;
        bus_config.apb1_prescaler = RCC_APB_DIV2;
        bus_config.apb2_prescaler = RCC_NOT_DIV;
        bus_config.usb_prescaler = USB_PRESCALER_DIV1;

        status = setup_system_config_rcc(&system_config);
        if (status != 0)
        {
            bus_config.adc_prescaler = RCC_NOT_DIV;
            bus_config.ahb_prescaler = RCC_NOT_DIV;
            bus_config.apb1_prescaler = RCC_NOT_DIV;
            bus_config.apb2_prescaler = RCC_NOT_DIV;

            system_config.source = RCC_HSI;
            system_config.pll_config = NULL;
            status = setup_system_config_rcc(&system_config);
        }
    }
    peripheral_conf.APB2 = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_USART1EN |  RCC_APB2ENR_SPI1EN;
    peripheral_conf.APB1 = RCC_APB1ENR_I2C1EN | RCC_APB1ENR_TIM4EN;
    peripheral_conf.AHB = RCC_AHBENR_FLITFEN | RCC_AHBENR_SRAMEN | RCC_AHBENR_DMA1EN;
    enable_gpio_clock_rcc(&peripheral_conf);
    return status;
}

int init_board()
{
    int status = 0;

    init_irq();

    status = init_rcc();

    init_led();

    if (status != 0)
    {
        return -1;
    }

    init_timer();

    init_uart();

    stm_init_log(usart_adapter);

    init_spi();

    return 0;
}

void print_clock_frequencies()
{
    RCC_Frequencies freq = {0};
    get_clock_frequencies(&freq);

    LOG_INFO("SYSCLK: %lu Hz\r\n", freq.SYSCLK_Freq);
    LOG_INFO("HCLK  : %lu Hz\r\n", freq.HCLK_Freq);
    LOG_INFO("APB1  : %lu Hz\r\n", freq.APB1_Freq);
    LOG_INFO("APB2  : %lu Hz\r\n", freq.APB2_Freq);
    LOG_INFO("USB  : %lu Hz\r\n", freq.USB_Freq);
    LOG_INFO("ADC  : %lu Hz\r\n", freq.ADC_Freq);
    LOG_INFO("source: %d\r\n", get_sysclk_source());
}
