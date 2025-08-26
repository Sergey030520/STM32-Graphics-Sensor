#include "tft_board.h"
#include "board_pins.h"
#include "gpio.h"
#include "memory_map.h"
#include "timer.h"
#include "spi.h"
#include <stdlib.h>
#include "rcc.h"
#include "log.h"

#define TFT_RES_Port GPIOA_REG
#define TFT_RES_Pin 1
#define TFT_DC_Port GPIOA_REG
#define TFT_DC_Pin 2
#define TFT_BLK_Port GPIOB_REG
#define TFT_BLK_Pin 6

#define SPI_TX_BUFFER_SIZE 512
#define SPI_RX_BUFFER_SIZE 512

TFT_Interface_t tft = {0};
static SPI_HandleTypeDef spi1_handle = {0};
PWM_Config_t pwm_cfg = {0};

static int init_spi();
int tft_spi_recv(uint8_t *data, uint32_t size);
int tft_spi_send(uint8_t *data, uint32_t size);

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
    set_pwm_timer(pwm_cfg.timer, value);
}

int init_spi()
{
    RCC_Frequencies freq = {0};
    get_clock_frequencies(&freq);

    enable_and_reset_rcc(RCC_BUS_APB2, RCC_APB2ENR_SPI1EN);

    // Настройка пинов SPI
    GPIO_PinConfig_t pin_sck_cfg = {
        .gpiox = SPI1_SCK_PORT,
        .pin = SPI1_SCK_PIN,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_AF_PP,
        .af_remap = 0,
    };

    GPIO_PinConfig_t pin_cs_cfg = {
        .gpiox = SPI1_CS_PORT,
        .pin = SPI1_CS_PIN,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_GP_PP,
        .af_remap = 0,
    };

    GPIO_PinConfig_t pin_miso_cfg = {
        .gpiox = SPI1_MISO_PORT,
        .pin = SPI1_MISO_PIN,
        .speed = GPIO_MODE_INPUT,
        .pin_mode = GPIO_INPUT_FLOATING,
        .af_remap = 0,
    };

    GPIO_PinConfig_t pin_mosi_cfg = {
        .gpiox = SPI1_MOSI_PORT,
        .pin = SPI1_MOSI_PIN,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_AF_PP,
        .af_remap = 0,
    };

    // Настройка DMA (mosi)
    spi1_handle.mosi_dma.dma = (DMA_Type *)DMA1_REG;
    spi1_handle.mosi_dma.channel = CHANNEL_3;
    spi1_handle.mosi_dma.direction = DMA_DIR_MEM_TO_PERIPH;
    spi1_handle.mosi_dma.mem_size = DMA_DATASIZE_8BIT;
    spi1_handle.mosi_dma.periph_size = DMA_DATASIZE_8BIT;
    spi1_handle.mosi_dma.inc_mem = 1;
    spi1_handle.mosi_dma.inc_periph = 0;
    spi1_handle.mosi_dma.circular = 0;

    // Настройка SPI_HandleTypeDef
    spi1_handle.spi = (SPI_Type *)SPI1_REG;
    spi1_handle.baud_rate = 20000000;
    spi1_handle.data_size = SPI_DATASIZE_8BIT;
    spi1_handle.cpha = SPI_CPHA_1EDGE;
    spi1_handle.cpol = SPI_CPOL_LOW;
    spi1_handle.nss = SPI_NSS_SOFT;
    spi1_handle.spi_mode = SPI_MASTER;
    spi1_handle.mosi_mode = SPI_MODE_POLLING;
    spi1_handle.miso_mode = SPI_MODE_DISABLE;

    SPI_Config_t cfg = {
        .spi = spi1_handle.spi,
        .baud_rate = spi1_handle.baud_rate,
        .data_size = spi1_handle.data_size,
        .cpha = spi1_handle.cpha,
        .cpol = spi1_handle.cpol,
        .nss = spi1_handle.nss,
        .spi_mode = spi1_handle.spi_mode,
        .miso_mode = spi1_handle.miso_mode,
        .mosi_mode = spi1_handle.mosi_mode,
        .miso_dma = NULL,
        .mosi_dma = &spi1_handle.mosi_dma,
        .pin_sck = pin_sck_cfg,
        .pin_miso = pin_miso_cfg,
        .pin_mosi = pin_mosi_cfg,
        .pin_cs = pin_cs_cfg,
    };
    return setup_spi(&cfg, freq.APB2_Freq);
}

int tft_spi_send(uint8_t *data, uint32_t size)
{
    if (!data || size == 0 || size > SPI_TX_BUFFER_SIZE)
        return -1;

    int status = send_data_spi_master(&spi1_handle, data, size);

    return status;
}

int tft_spi_recv(uint8_t *data, uint32_t size)
{
    if (!data || size == 0 || size > SPI_RX_BUFFER_SIZE)
        return -1;

    int status = recv_data_spi_master(&spi1_handle, data, size);

    return status;
}


void tft_init_board_interface()
{

    GPIO_PinConfig_t pin_blk = {
        .gpiox = GPIOB_REG,
        .pin = 6,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_AF_PP,
        .af_remap = GPIO_AFIO_DEFAULT,
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
    set_gpio_conf(&pin_blk);
    set_gpio_conf(&pin_dc);
    set_gpio_conf(&pin_res);

    int status = init_spi();
    if (status != 0)
    {
        return -2;
    }

    pwm_cfg.timer = (GP_Timer_Type *)TIM4_REG;
    pwm_cfg.channel = TIM_CHANNEL1;
    pwm_cfg.duty_cycle = 50; // 50% скважность
    pwm_cfg.pwm_freq = 20000; // 20 кГц

    RCC_Frequencies freq = {0};
    get_clock_frequencies(&freq);
    init_pwm_timer(&pwm_cfg, freq.APB1_Freq);

    tft.set_dc = tft_set_dc;
    tft.set_res = tft_set_res;
    tft.set_blk = tft_set_blk;
    tft.spi_send = tft_spi_send;
    tft.spi_recv = tft_spi_recv;
    tft.delay_ms = tft_delay_ms;
    tft.set_brightness = tft_set_brightness;

    st7789_init(&tft);
}
