#pragma once 


#include <stdint.h>
#include "gpio.h"
#include "dma.h"

typedef struct{
    uint32_t CR1;
    uint32_t CR2;
    uint32_t OAR1;
    uint32_t OAR2;
    uint32_t DR;
    uint32_t SR1;
    uint32_t SR2;
    uint32_t CCR;
    uint32_t TRISE;
}I2C_Type;


// CR1
#define I2C_CR1_SWRST (0x1 << 15)
#define I2C_CR1_ALERT (0x1 << 13)
#define I2C_CR1_PEC (0x1 << 12)
#define I2C_CR1_POS (0x1 << 11)
#define I2C_CR1_ACK (0x1 << 10)
#define I2C_CR1_STOP (0x1 << 9)
#define I2C_CR1_START (0x1 << 8)
#define I2C_CR1_NOSTRETCH (0x1 << 7)
#define I2C_CR1_ENGC (0x1 << 6)
#define I2C_CR1_ENPEC (0x1 << 5)
#define I2C_CR1_ENARP (0x1 << 4)
#define I2C_CR1_SMBTYPE (0x1 << 3)
#define I2C_CR1_SMBUS (0x1 << 1)
#define I2C_CR1_PE (0x1 << 0)

// CR2
#define I2C_CR2_LAST (0x1 << 13)
#define I2C_CR2_DMAEN (0x1 << 12)
#define I2C_CR2_ITBUFEN (0x1 << 11)
#define I2C_CR2_ITEVTEN (0x1 << 10)
#define I2C_CR2_ITERREN (0x1 << 9)

// OAR1
#define I2C_CR2_ADDMODE (0x1 << 15)

// SR 1
#define I2C_SR1_SMB_ALERT (0x1 << 15)
#define I2C_SR1_TIMEOUT (0x1 << 14)
#define I2C_SR1_PECERR (0x1 << 12)
#define I2C_SR1_OVR (0x1 << 11)
#define I2C_SR1_AF (0x1 << 10)
#define I2C_SR1_ARLO (0x1 << 9)
#define I2C_SR1_BERR (0x1 << 8)
#define I2C_SR1_TXE (0x1 << 7)
#define I2C_SR1_RXNE (0x1 << 6)
#define I2C_SR1_STOPF (0x1 << 4)
#define I2C_SR1_ADD10 (0x1 << 3)
#define I2C_SR1_BTF (0x1 << 2)
#define I2C_SR1_ADDR (0x1 << 1)
#define I2C_SR1_SB (0x1 << 0)


// SR 2
#define I2C_SR2_DUALF (0x1 << 7)
#define I2C_SR2_SMBHOST (0x1 << 6)
#define I2C_SR2_SMBDEFAULT (0x1 << 5)
#define I2C_SR2_GENCALL (0x1 << 4)
#define I2C_SR2_TRA (0x1 << 2)
#define I2C_SR2_BUSY (0x1 << 1)
#define I2C_SR2_MSL (0x1 << 0)




// Режим работы I2C
typedef enum
{
    I2C_MODE_DISABLE = 0,  // I2C отключен
    I2C_MODE_POLLING,      // Обычный режим опроса
    I2C_MODE_DMA           // Режим с использованием DMA
} I2C_Mode_t;

typedef enum
{
    I2C_ROLE_MASTER = 0,
    I2C_ROLE_SLAVE
} I2C_Role_t;

// Конфигурация I2C
typedef struct
{
    I2C_Type *instance;        // Указатель на периферию I2C (I2C1, I2C2 и т.д.)
    I2C_Mode_t mode;           // Режим работы (polling / DMA)
    I2C_Role_t role;           // Master или Slave
    uint32_t clock_speed;      // Тактовая частота шины (для master)
    uint8_t address;           // Адрес устройства (для slave)
    DMA_Config dma_tx;         // DMA конфигурация передачи (если используется)
    DMA_Config dma_rx;         // DMA конфигурация приёма (если используется)
    GPIO_PinConfig_t *scl_pin; // Пин SCL
    GPIO_PinConfig_t *sda_pin; // Пин SDA
} I2C_Config_t;


typedef struct
{
    I2C_Type *instance;      // Указатель на периферию I2C
    I2C_Role_t role;         // Master или Slave
    uint8_t address;         // Адрес устройства (только для slave)
    I2C_Mode_t mode;         // Режим работы (polling / DMA)
     DMA_Config dma_tx;         // DMA конфигурация передачи (если используется)
    DMA_Config dma_rx;         // DMA конфигурация приёма (если используется)
} I2C_HandleTypeDef;



I2C_HandleTypeDef setup_i2c(I2C_Config_t *cfg, uint32_t pclk1_mhz);
int send_data_i2c(I2C_HandleTypeDef *hi2c, uint8_t data);
int send_address_i2c(I2C_HandleTypeDef *hi2c, uint8_t address);
int write_data_i2c(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t data);
int read_data_i2c(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *data);
int read_multi_data_i2c(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *buffer, uint8_t length);


// irq
void I2C1_EV_IRQHandler();
void I2C1_ER_IRQHandler();
