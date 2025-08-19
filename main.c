#include <stdio.h>
#include "rcc.h"
#include "led.h"
#include "usart.h"
#include "dma.h"
#include "board_pins.h"
// #include "spi.h"
// #include "tft.h"
#include "IRQ.h"
#include "log.h"
#include "timer.h"



int init_rcc();
int init_board();
int usart_adapter(const char *data, int length);
void init_uart();

int main()
{

    int status = init_board();

    if (status != 0)
    {
        init_led();
        ledOn(1);
        goto error;
    }
    LOG_INFO("Драйверы запущены");
    ledOn(0);

    // init_spi_master();
 
    // st7789_init();

    //
    // 63 g, 31 r, 31 - b
    // uint16_t color = 0xF8;

    // fill_color_display(RGB565(0, 0, 255));
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
        LOG_INFO("STMf103");
        // delay_timer(2000);
        return 0;
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
    peripheral_conf.APB2 = RCC_IOPCEN | RCC_IOPAEN | RCC_IOPBEN | RCC_USART1EN;
    peripheral_conf.APB1 = RCC_I2C1EN;
    peripheral_conf.AHB = RCC_FLITFEN | RCC_SRAMEN | RCC_DMA1EN;
    enable_gpio_clock_rcc(&peripheral_conf);
    return status;
}

int init_board()
{
    int status = 0;
    RCC_Frequencies rcc_clocks = {0};

    init_irq();

    status = init_rcc();

    init_led();

    if (status != 0)
    {
        ledOn(1);
        return -1;
    }


    init_timer();

    init_uart();

    stm_init_log(usart_adapter);


    return 0;
}

int usart_adapter(const char *data, int length)
{
    send_data_usart((uint8_t *)data, (uint16_t)length);
    return 0;
}

void init_uart()
{
    DMA_Config dma_tx_config = {
        .dma = DMA1_REG,         
        .channel = CHANNEL_4, 
        .direction = DMA_DIR_MEM_TO_PERIPH,
        .mem_size = DMA_MSIZE_8BITS,
        .periph_size = DMA_PSIZE_8BITS,
        .inc_mem = 1,
        .inc_periph = 0,
        .circular = 0};

    GPIO_PinConfig_t tx_pin_config = {
        .gpiox = USART1_TX_PORT,
        .pin = USART1_TX_PIN,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_AF_PP,
        .af_remap =  0,
    };
    GPIO_PinConfig_t rx_pin_config = {
        .gpiox = USART1_RX_PORT,
        .pin = USART1_RX_PIN,
        .speed = GPIO_MODE_INPUT,
        .pin_mode = GPIO_INPUT_PUPD,
        .af_remap =  0,
    };

    // Основная конфигурация UART
    UART_Config_t uart1_config = {
        .usart = USART1_REG,
        .baud_rate = UART_BAUDRATE_9600,
        .tx_mode = UART_MODE_POLLING,
        .rx_mode = UART_MODE_POLLING,
        .dma_tx = dma_tx_config,   // DMA конфигурация для TX
        .dma_rx = {0},             // RX DMA не используется
        .tx_port = &tx_pin_config, // Пин TX
        .rx_port = &rx_pin_config  // Пин RX (можно NULL, если RX отключен)
    };

    RCC_Frequencies rcc_clocks = {0};

    get_clock_frequencies(&rcc_clocks);

    setup_uart(&uart1_config, rcc_clocks.APB2_Freq);
}
