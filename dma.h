#include <stdint.h>

#pragma once

#define DMA1_Reg 0x40020000U
#define DMA2_Reg 0x40020400U



// CCR
#define DMA_MEM2MEM (0x1 << 14)
#define DMA_PL_LOW (0b00 << 12)
#define DMA_PL_MEDIUM (0b01 << 12)
#define DMA_PL_HIGH (0b10 << 12)
#define DMA_PL_VERY_HIGH (0b11 << 12)
#define DMA_MSIZE_8BITS (0x00 << 10)
#define DMA_MSIZE_16BITS (0x01 << 10)
#define DMA_MSIZE_32BITS (0x10 << 10)
#define DMA_PSIZE_8BITS (0x00 << 8)
#define DMA_PSIZE_16BITS (0x01 << 8)
#define DMA_PSIZE_32BITS (0x10 << 8)
#define DMA_MINC (0x1 << 7)
#define DMA_PINC (0x1 << 6)
#define DMA_CIRC (0x1 << 5)
#define DMA_DIR_MEMORY (0x1 << 4)
#define DMA_DIR_PERIPHERAL (0x0 << 4)
#define DMA_TEIE (0x1 << 3)
#define DMA_HTIE (0x1 << 2)
#define DMA_TCIE (0x1 << 1)
#define DMA_EN (0x1 << 0)

// SR
#define TEIFx(channel) (0x1 << ((channel - 1) * 4 + 3))
#define HTIFx(channel) (0x1 << ((channel - 1) * 4 + 2))
#define TCIFx(channel) (0x1 << ((channel - 1) * 4 + 1))
#define GIFx(channel) (0x1 << ((channel - 1) * 4))

// IFSR
#define CTEIFx TEIFx
#define CHTIFx HTIFx
#define CTCIFx TCIFx
#define CGIFx GIFx

typedef enum
{
    CHANNEL_1 = 1,
    CHANNEL_2 = 2,
    CHANNEL_3 = 3,
    CHANNEL_4 = 4,
    CHANNEL_5 = 5,
    CHANNEL_6 = 6,
    CHANNEL_7 = 7,
} DMA_Channel;


typedef struct
{
    uint32_t CCR;   // конфигурация канала
    uint32_t CNDTR; // кол-во передаваемых данных
    uint32_t CPAR;  // адрес периферии канала
    uint32_t CMAR;  // адрес памяти канала
    uint32_t reserved;
} DMA_Channel_Settings;

typedef struct
{
    uint32_t ISR;
    uint32_t IFCR;
    DMA_Channel_Settings channels[7];
} DMA_Type;



extern DMA_Type *dma1;


void send_data_usart1_dma(uint8_t *message, uint16_t length);
void recive_data_usart1_dma(uint8_t *message, uint16_t length);








void DMA1_Channel3_IRQHandler(void);
