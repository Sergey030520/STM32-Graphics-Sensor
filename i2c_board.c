#include "i2c_board.h"
#include "rcc.h"
#include "dma.h"
#include "gpio.h"
#include "memory_map.h"
#include "board_pins.h"


I2C_HandleTypeDef i2c_handle = {0};

// int usart_adapter(const char *data, int length)
// {
//     return 0;
// }


void board_init_i2c()
{
    DMA_Config dma_tx_config = {
        .dma = DMA1_REG,
        .channel = CHANNEL_4,
        .direction = DMA_DIR_MEM_TO_PERIPH,
        .mem_size = DMA_DATASIZE_8BIT,
        .periph_size = DMA_DATASIZE_8BIT,
        .inc_mem = 1,
        .inc_periph = 0,
        .circular = 0
    };

    GPIO_PinConfig_t sda_pin_config = {
        .gpiox = I2C1_SDA_PORT,
        .pin = I2C1_SDA_PIN,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_AF_OD,   // открытый сток!
        .af_remap = 0,
    };

    GPIO_PinConfig_t scl_pin_config = {
        .gpiox = I2C1_SCL_PORT,
        .pin = I2C1_SCL_PIN,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_AF_OD,   // тоже открытый сток
        .af_remap = 0,
    };

    I2C_Config_t i2c_cfg = {
        .instance = I2C1_REG,
        .sda_pin = &sda_pin_config,
        .scl_pin = &scl_pin_config,
        .clock_speed = 100000,        // стандартный режим 100 кГц
        .address = 0,                 // для slave = адрес, для master = 0
        .role = I2C_ROLE_MASTER,
        .mode = I2C_MODE_POLLING,     // или I2C_MODE_DMA
        .dma_tx = dma_tx_config,      // передаём сконфигурированный DMA
        .dma_rx = {0},                // если нужно — настроить RX DMA
    };

    RCC_Frequencies rcc_clocks = {0};
    get_clock_frequencies(&rcc_clocks);

    i2c_handle = setup_i2c(&i2c_cfg, rcc_clocks.APB1_Freq);
}
