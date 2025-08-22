#include <stdint.h>

#pragma once

// CCR
#define DMA_CCR_MEM2MEM (0x1 << 14)
#define DMA_CCR_PL(priority) ((priority & 0x3) << 12)
#define DMA_CCR_MSIZE(size) ((size & 0x3) << 10)
#define DMA_CCR_PSIZE(size) ((size & 0x3) << 8)
#define DMA_CCR_MINC (0x1 << 7)
#define DMA_CCR_PINC (0x1 << 6)
#define DMA_CCR_CIRC (0x1 << 5)
#define DMA_CCR_DIR(dir) ((dir & 0x1) << 4)
#define DMA_CCR_TEIE (0x1 << 3)
#define DMA_CCR_HTIE (0x1 << 2)
#define DMA_CCR_TCIE (0x1 << 1)
#define DMA_CCR_EN (0x1 << 0)

// SR
#define DMA_SR_TEIFx(channel) (0x1 << ((channel) * 4 + 3))
#define DMA_SR_HTIFx(channel) (0x1 << ((channel) * 4 + 2))
#define DMA_SR_TCIFx(channel) (0x1 << ((channel) * 4 + 1))
#define DMA_SR_GIFx(channel) (0x1 << ((channel) * 4))

// IFSR
#define DMA_IFCR_CTEIFx DMA_SR_TEIFx
#define DMA_IFCR_CHTIFx DMA_SR_HTIFx
#define DMA_IFCR_CTCIFx DMA_SR_TCIFx
#define DMA_IFCR_CGIFx DMA_SR_GIFx

typedef enum
{
    CHANNEL_1 = 0,
    CHANNEL_2 = 1,
    CHANNEL_3 = 2,
    CHANNEL_4 = 3,
    CHANNEL_5 = 4,
    CHANNEL_6 = 5,
    CHANNEL_7 = 6,
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

typedef enum
{
    DMA_DIR_PERIPH_TO_MEM = 0,
    DMA_DIR_MEM_TO_PERIPH = 1,
} DMA_Direction;

typedef enum
{
    DMA_DATASIZE_8BIT = 0x0,
    DMA_DATASIZE_16BIT = 0x1,
    DMA_DATASIZE_32BIT = 0x2
} DMA_DataSize;

typedef enum
{
    DMA_PRIORITY_LOW = 0x0,
    DMA_PRIORITY_MEDIUM = 0x1,
    DMA_PRIORITY_HIGH = 0x2,
    DMA_PRIORITY_VERY_HIGH = 0x3
} DMA_ChannelPriority;

typedef struct
{
    DMA_Type *dma;
    DMA_Channel channel;
    DMA_Direction direction;
    uint32_t peripheral_addr;
    uint32_t mem_addr;
    DMA_DataSize mem_size;
    DMA_DataSize periph_size;
    DMA_ChannelPriority priority;
    uint8_t inc_mem;
    uint8_t inc_periph;
    uint8_t circular;
    uint32_t length;
} DMA_Config;

typedef void (*dma_callback_t)(void);

void dma_init(DMA_Config *cfg);
void dma_start(DMA_Config *cfg);
uint8_t dma_transfer_complete(DMA_Type *dma, DMA_Channel channel);

void DMA1_Channel1_IRQHandler();
void DMA1_Channel2_IRQHandler();
void DMA1_Channel3_IRQHandler();
void DMA1_Channel4_IRQHandler();
void DMA1_Channel5_IRQHandler();
void DMA1_Channel6_IRQHandler();
void DMA1_Channel7_IRQHandler();


void dma_set_memory(DMA_Config *cfg, uint32_t mem_addr, uint32_t length);

// uint8_t dma_transfer_complete(DMA_Config *cfg);
void reset_flags_dma(DMA_Type *dma, DMA_Channel channel);
void dma_config_transfer(DMA_Config *cfg, uint32_t periph_addr, uint32_t mem_addr, uint32_t length);
