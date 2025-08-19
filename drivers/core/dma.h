#include <stdint.h>

#pragma once

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
#define DMA_SR_TEIFx(channel) (0x1 << ((channel - 1) * 4 + 3))
#define DMA_SR_HTIFx(channel) (0x1 << ((channel - 1) * 4 + 2))
#define DMA_SR_TCIFx(channel) (0x1 << ((channel - 1) * 4 + 1))
#define DMA_SR_GIFx(channel) (0x1 << ((channel - 1) * 4))

// IFSR
#define DMA_IFCR_CTEIFx DMA_SR_TEIFx
#define DMA_IFCR_CHTIFx DMA_SR_HTIFx
#define DMA_IFCR_CTCIFx DMA_SR_TCIFx
#define DMA_IFCR_CGIFx DMA_SR_GIFx

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

typedef enum {
    DMA_DIR_PERIPH_TO_MEM = 0,
    DMA_DIR_MEM_TO_PERIPH = 1,
} DMA_Direction;

typedef struct {
    DMA_Type *dma;
    DMA_Channel channel;
    DMA_Direction direction;
    uint32_t peripheral_addr;
    uint32_t mem_size; 
    uint32_t periph_size;
    uint8_t inc_mem;
    uint8_t inc_periph;
    uint8_t circular;
} DMA_Config;

typedef void (* dma_callback_t) (void);


void dma_init(DMA_Config *cfg);
void dma_send(DMA_Config *cfg, void *buffer, uint32_t length);

void set_enabled_dma(DMA_Type *dma, DMA_Channel channel, uint8_t enable);
void reset_flags_dma(DMA_Type *dma, DMA_Channel channel);


void DMA2_Stream3_IRQHandler();
void DMA2_Stream4_IRQHandler();
void DMA2_Stream7_IRQHandler();