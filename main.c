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
#include "sensors.h"
#include "string.h"
#include "gfx.h"



int init_rcc();
int init_board();
void print_clock_frequencies();

void display_sensor_data(Vector *vector);



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

    sensors_init();

    gfx_drv_init();

    Vector vector = {0};

    while (1)
    {
        status = get_data_sensor_sc7a20h(&vector);
        if (status != 0)
        {
            LOG_INFO("Error get data sensor: %d\r\n", status);
        }
        else
        {
            display_sensor_data(&vector);
            LOG_INFO("Sensor SC7A20H: {x:%d,y:%d,z:%d}\r\n", vector.X, vector.Y, vector.Z); 
        }
        delay_timer(5000);
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
    peripheral_conf.APB2 = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_USART1EN | RCC_APB2ENR_SPI1EN;
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

    board_init_uart();

    stm_init_log(usart_adapter);

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

void display_sensor_data(Vector *vector)
{
    char buffer[1024];

    snprintf(buffer, 1024,
                       "Sensor SC7A20H: {x:%d, y:%d, z:%d}",
                       vector->X, vector->Y, vector->Z);

    gfx_drv_clear();
    gfx_drv_draw_string(0, 0, buffer); 
    gfx_drv_update();
}