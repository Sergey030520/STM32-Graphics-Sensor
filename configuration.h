#include <stdint.h>

#pragma once


#define FLASH_REG 0x40022000U
#define GPIOC_REG 0x40011000U
#define GPIOA_REG 0x40010800U
#define GPIOB_REG 0x40010C00U
#define AFIO_REG 0x40010000U


// Flash memory
typedef struct{
    uint32_t ACR;
    uint32_t KEYR;
    uint32_t OPTKEYR;
    uint32_t SR;
    uint32_t CR;
    uint32_t AR;
    uint32_t reserved;
    uint32_t OBR;
    uint32_t WRPR;
} FLASH_Type;

#define FLASH_LATENCY_ZERO (0b000 << 0)
#define FLASH_LATENCY_ONE (0b001 << 0)
#define FLASH_LATENCY_TWO (0b010 << 0)

// GPIO

typedef struct {
    uint32_t CRL;
    uint32_t CRH;
    uint32_t IDR;
    uint32_t ODR;
    uint32_t BSRR;
    uint32_t BRR;
    uint32_t LCKR;
}GPIOx;


// input cnf 
#define GPIO_ANALOG_MODE 0b00
#define GPIO_FLOATING_INPUT 0b01
#define GPIO_INPUT_PULL_UP 0b10
#define GPIO_INPUT_PULL_DOWN GPIO_INPUT_PULL_UP 

// output cnf

#define GPIO_OUTPUT_PUSH_PULL 0b00 
#define GPIO_OUTPUT_OPEN_DRAIN 0b01
#define GPIO_AF_PUSH_PULL 0b10 
#define GPIO_AF_OPEN_DRAIN 0b11
 
// mode
#define GPIO_MODE_INPUT 0b00 
#define GPIO_MODE_SPEED_10MHz 0b01
#define GPIO_MODE_SPEED_2MHz 0b10
#define GPIO_MODE_SPEED_50MHz 0b11


#define GPIO_MASK_PORT_CR(port) ((uint64_t)0b1111 << (port * 4))


#define GPIO_MASK_PORT_CRL(port) (0b1111 << (port * 4))
#define GPIO_MASK_PORT_CRH(port) (0b1111 << ((port - 8) * 4))

#define GPIO_CRH_CNF_MODE(port, mode, cnf) ((cnf << (4 * (port-8) + 2)) | (mode << (4 * (port-8))))
#define GPIO_CRL_CNF_MODE(port, mode, cnf) ((cnf << (4 * port + 2)) | (mode << (4 * port)))

#define GPIO_CNF_MODE(port, mode, cnf) (((uint64_t)cnf << (4 * port + 2)) | ((uint64_t)mode << (4 * port)))


typedef struct{
    uint32_t ECVR;
    uint32_t MAPR;
    uint32_t EXTICR1;
    uint32_t EXTICR2;
    uint32_t EXTICR3;
    uint32_t EXTICR4;
    uint32_t MAPR2;
} AFIO_Type;

#define AFIO_USART1_NO_REMAP (0x0 << 2) 
#define AFIO_USART1_REMAP (0x1 << 2) 


// NVECTOR

#define NVIC_REG 0xE000E004U

typedef struct
{
    uint32_t ICTR; // Interrupt Controller Type Register
    uint32_t RESERV1[62];
    uint32_t ISERx[8]; // Interrupt Set-Enable Registers
    uint32_t RESERV2[24];
    uint32_t ICERx[8]; // Interrupt Clear-Enable Registers
    uint32_t RESERV3[24];
    uint32_t ISPRx[8]; // Interrupt Set-Pending Registers
    uint32_t RESERV4[24];
    uint32_t ICPRx[8]; // Interrupt Clear-Pending Registers
    uint32_t RESERV5[24];
    uint32_t IABRx[8]; // Interrupt Active Bit Register
    uint32_t RESERV6[56];
    uint32_t IPRx[60]; // Interrupt Priority Register
} NVIC_Type;


#define NVIC_DMA1_CHANNEL3 (0x1 << 13)
#define NVIC_DMA1_CHANNEL4 (0x1 << 14)
#define NVIC_DMA1_CHANNEL5 (0x1 << 15)
#define NVIC_SPI1 (0x1 << 3)
#define NVIC_SPI2 (0x1 << 4)
#define NVIC_USART1 (0x1 << 5)
#define NVIC_SPI3 (0x1 << 19)


extern GPIOx *gpioa;
extern GPIOx *gpiob;
extern GPIOx *gpioc;

