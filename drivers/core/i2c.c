#include "i2c.h"
#include <stdio.h>


#define I2C_WRITE 0x0
#define I2C_READ 0x1
#define I2C_TIMEOUT 100000U



int i2c_wait_flag(I2C_HandleTypeDef *hi2c, uint32_t flag_mask, uint32_t timeout)
{
    while (!(hi2c->instance->SR1 & flag_mask))
    {
        if (--timeout == 0)
            return -1;
    }
    return 0;
}


I2C_HandleTypeDef setup_i2c(I2C_Config_t *cfg, uint32_t pclk1_mhz)
{
    I2C_HandleTypeDef hi2c;

    // Сохраняем минимальные данные в handle
    hi2c.instance = cfg->instance;
    hi2c.role = cfg->role;
    hi2c.mode = cfg->mode;
    hi2c.address = cfg->address;
    hi2c.dma_tx = cfg->dma_tx;
    hi2c.dma_rx = cfg->dma_rx;

    // Настройка GPIO
    set_gpio_conf(cfg->scl_pin);
    set_gpio_conf(cfg->sda_pin);

    I2C_Type *i2c = cfg->instance;

    // Сброс I2C
    i2c->CR1 = 0;
    i2c->CR1 = I2C_CR1_SWRST;
    i2c->CR1 &= ~I2C_CR1_SWRST;

    // Настройка частоты шины (только для master)
    i2c->CR2 &= ~0x3F;
    i2c->CR2 |= (pclk1_mhz & 0x3F);

    if(cfg->role == I2C_ROLE_MASTER)
    {
        // Расчёт CCR и TRISE для master
        uint32_t ccr = pclk1_mhz * 1000000 / (2 * cfg->clock_speed);
        i2c->CCR = ccr;
        i2c->TRISE = pclk1_mhz + 1; // стандартная формула для sm mode
    }
    else // Slave
    {
        i2c->OAR1 = (cfg->address << 1) & 0x7F;
    }

    // Включаем I2C и ACK
    i2c->CR1 |= I2C_CR1_PE | I2C_CR1_ACK;

    return hi2c;
}


int send_data_i2c(I2C_HandleTypeDef *hi2c, uint8_t data)
{
    if (i2c_wait_flag(hi2c->instance, I2C_SR1_TXE, I2C_TIMEOUT))
        return -1;

    hi2c->instance->DR = data;

    if (i2c_wait_flag(hi2c->instance, I2C_SR1_BTF, I2C_TIMEOUT))
        return -2;

    return 0;
}

int send_address_i2c(I2C_HandleTypeDef *hi2c, uint8_t address)
{
    hi2c->instance->DR = address;

    uint32_t timeout = I2C_TIMEOUT;
    while (!(hi2c->instance->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF)))
    {
        if (--timeout == 0)
            return -1;
    }

    if (hi2c->instance->SR1 & I2C_SR1_AF)
    {
        hi2c->instance->SR1 &= ~I2C_SR1_AF;
        return -2;
    }

    volatile uint32_t temp = hi2c->instance->SR1;
    temp = hi2c->instance->SR2; // чтение для очистки флага ADDR

    (void)temp;
    return 0;
}

int write_data_i2c(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t data)
{
    int status = 0;

    hi2c->instance->CR1 |= I2C_CR1_START;

    status = i2c_wait_flag(hi2c->instance, I2C_SR1_SB, I2C_TIMEOUT);
    if (status != 0)
        return -1;

    status = send_address_i2c(hi2c, (hi2c->address << 1) | I2C_WRITE);
    if (status != 0)
        return -2;

    status = send_data_i2c(hi2c, reg);
    if (status != 0)
        return -3;

    status = send_data_i2c(hi2c, data);
    if (status != 0)
        return -4;

    hi2c->instance->CR1 |= I2C_CR1_STOP;

    return 0;
}

int read_data_i2c(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *data)
{
    if (data == NULL)
        return -1;

    int status = 0;

    hi2c->instance->CR1 |= I2C_CR1_START;
    status = i2c_wait_flag(hi2c->instance, I2C_SR1_SB, I2C_TIMEOUT);
    if (status != 0)
        return -2;

    status = send_address_i2c(hi2c, (hi2c->address << 1) | I2C_WRITE);
    if (status != 0)
        return -3;

    status = send_data_i2c(hi2c, reg);
    if (status != 0)
        return -4;

    hi2c->instance->CR1 |= I2C_CR1_START;
    status = i2c_wait_flag(hi2c->instance, I2C_SR1_SB, I2C_TIMEOUT);
    if (status != 0)
        return -5;

    hi2c->instance->CR1 &= ~I2C_CR1_ACK;

    status = send_address_i2c(hi2c, (hi2c->address << 1) | I2C_READ);
    if (status != 0)
        return -6;

    hi2c->instance->CR1 |= I2C_CR1_STOP;

    status = i2c_wait_flag(hi2c->instance, I2C_SR1_RXNE, I2C_TIMEOUT);
    if (status != 0)
        return -7;

    *data = hi2c->instance->DR;

    hi2c->instance->CR1 |= I2C_CR1_ACK;

    return 0;
}

int read_multi_data_i2c(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *buffer, uint8_t length)
{
    if (buffer == NULL || length == 0)
        return -1;

    int status = 0;

    hi2c->instance->CR1 |= I2C_CR1_START;
    status = i2c_wait_flag(hi2c->instance, I2C_SR1_SB, I2C_TIMEOUT);
    if (status != 0)
        return -2;

    status = send_address_i2c(hi2c, (hi2c->address << 1) | I2C_WRITE);
    if (status != 0)
        return -3;

    status = send_data_i2c(hi2c, reg);
    if (status != 0)
        return -4;

    hi2c->instance->CR1 |= I2C_CR1_START;
    status = i2c_wait_flag(hi2c->instance, I2C_SR1_SB, I2C_TIMEOUT);
    if (status != 0)
        return -5;

    status = send_address_i2c(hi2c, (hi2c->address << 1) | I2C_READ);
    if (status != 0)
        return -6;

    if (length == 1)
    {
        hi2c->instance->CR1 &= ~I2C_CR1_ACK;
        hi2c->instance->CR1 |= I2C_CR1_STOP;

        status = i2c_wait_flag(hi2c->instance, I2C_SR1_RXNE, I2C_TIMEOUT);
        if (status != 0)
            return -7;

        buffer[0] = hi2c->instance->DR;
    }
    else
    {
        hi2c->instance->CR1 |= I2C_CR1_ACK;

        for (uint8_t i = 0; i < length; i++)
        {
            if (i == length - 2)
            {
                hi2c->instance->CR1 &= ~I2C_CR1_ACK;
            }

            status = i2c_wait_flag(hi2c->instance, I2C_SR1_RXNE, I2C_TIMEOUT);
            if (status != 0)
                return -8;

            buffer[i] = hi2c->instance->DR;

            if (i == length - 2)
            {
                hi2c->instance->CR1 |= I2C_CR1_STOP;
            }
        }
    }

    return 0;
}
