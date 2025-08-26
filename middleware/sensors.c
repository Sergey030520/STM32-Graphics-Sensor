#include "sensors.h"
#include "rcc.h"
#include "dma.h"
#include "gpio.h"
#include "memory_map.h"
#include "board_pins.h"
#include "i2c.h"
#include "SC7A20H.h"
#include "log.h"
#include "timer.h"

I2C_HandleTypeDef i2c_handle = {0};
SC7A20H_Handle_t sensor_sc7a20h = {0};

int sensors_i2c_write(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len);
int sensors_i2c_read(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len);
int sensors_i2c_init();

int sensors_i2c_write(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len)
{
    if (!data || len == 0)
        return -1;

    if (len == 1)
        return write_data_i2c(&i2c_handle, dev_addr, reg, *data);

    return write_multi_data_i2c(&i2c_handle, dev_addr, reg, data, len);
}

int sensors_i2c_read(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len)
{
    if (len == 1)
    {
        return read_data_i2c(&i2c_handle, dev_addr, reg, data);
    }
    return read_multi_data_i2c(&i2c_handle, dev_addr, reg, data, len);
}

int sensors_i2c_init()
{
    DMA_Config dma_tx_config = {
        .dma = DMA1_REG,
        .channel = CHANNEL_4,
        .direction = DMA_DIR_MEM_TO_PERIPH,
        .mem_size = DMA_DATASIZE_8BIT,
        .periph_size = DMA_DATASIZE_8BIT,
        .inc_mem = 1,
        .inc_periph = 0,
        .circular = 0};

    GPIO_PinConfig_t sda_pin_config = {
        .gpiox = I2C1_SDA_PORT,
        .pin = I2C1_SDA_PIN,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_AF_OD,
        .af_remap = 0,
    };

    GPIO_PinConfig_t scl_pin_config = {
        .gpiox = I2C1_SCL_PORT,
        .pin = I2C1_SCL_PIN,
        .speed = GPIO_OUTPUT_50MHz,
        .pin_mode = GPIO_OUTPUT_AF_OD,
        .af_remap = 0,
    };

    I2C_Config_t i2c_cfg = {
        .instance = I2C1_REG,
        .sda_pin = &sda_pin_config,
        .scl_pin = &scl_pin_config,
        .clock_speed = I2C_SPEED_STANDARD,
        .address = 0,
        .role = I2C_ROLE_MASTER,
        .mode = I2C_MODE_POLLING, // или I2C_MODE_DMA
        .dma_tx = dma_tx_config,  // передаём сконфигурированный DMA
        .dma_rx = {0},            // если нужно — настроить RX DMA
    };

    RCC_Frequencies rcc_clocks = {0};
    get_clock_frequencies(&rcc_clocks);

    setup_i2c(&i2c_handle, &i2c_cfg, rcc_clocks.APB1_Freq);

    return 0;
}

void sensors_init()
{
    sensors_i2c_init();

    sensor_sc7a20h.read = sensors_i2c_read;
    sensor_sc7a20h.write = sensors_i2c_write;
    sensor_sc7a20h.sensitivity = 0;
    sensor_sc7a20h.odr = 50;
    sensor_sc7a20h.dev_addr = SC7A20H_ADDRES_I2C;
    sensor_sc7a20h.is_initialized = 0;

    // SC7A20H
    int status = init_sc7a20h(&sensor_sc7a20h);
    if (status != 0)
    {
        LOG_INFO("Init error sensor: %d\r\n", status);
    }
}

int get_data_sensor_sc7a20h(Vector *vector)
{
    if (!vector)
        return -1;

    SC7A20H_Data_t data;
    int status = read_xyz_sc7a20h(&sensor_sc7a20h, &data);
    if (status != 0)
        return status;

    vector->X = data.X;
    vector->Y = data.Y;
    vector->Z = data.Z;

    return 0;
}