#pragma once

#include "gpio.h"
#include "memory_map.h"

// --- LED ---
#define LED_PORT       GPIOB_REG
#define LED_PIN        13

// --- USART1 ---
#define USART1_TX_PORT GPIOA_REG
#define USART1_TX_PIN  9
#define USART1_RX_PORT GPIOA_REG
#define USART1_RX_PIN  10

// --- SPI1 ---
#define SPI1_SCK_PORT  GPIOA_REG
#define SPI1_SCK_PIN   5
#define SPI1_MISO_PORT GPIOA_REG
#define SPI1_MISO_PIN  6
#define SPI1_MOSI_PORT GPIOA_REG
#define SPI1_MOSI_PIN  7
#define SPI1_CS_PORT   GPIOA_REG
#define SPI1_CS_PIN    4

// --- SPI2 ---
#define SPI2_SCK_PORT  GPIOB_REG
#define SPI2_SCK_PIN   13
#define SPI2_MISO_PORT GPIOB_REG
#define SPI2_MISO_PIN  14
#define SPI2_MOSI_PORT GPIOB_REG
#define SPI2_MOSI_PIN  15
#define SPI2_CS_PORT   GPIOB_REG
#define SPI2_CS_PIN    12

// --- I2C1 ---
#define I2C1_SCL_PORT  GPIOB_REG
#define I2C1_SCL_PIN   6
#define I2C1_SDA_PORT  GPIOB_REG
#define I2C1_SDA_PIN   7

// --- I2C2 ---
#define I2C2_SCL_PORT  GPIOB_REG
#define I2C2_SCL_PIN   10
#define I2C2_SDA_PORT  GPIOB_REG
#define I2C2_SDA_PIN   11


// --- USB (FS) ---
#define USB_DM_PORT    GPIOA_REG
#define USB_DM_PIN     11
#define USB_DP_PORT    GPIOA_REG
#define USB_DP_PIN     12
