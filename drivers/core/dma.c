#include "dma.h"
#include "memory_map.h"
#include <stdio.h>

DMA_Type *DMA1 = DMA1_REG;
DMA_Type *DMA2 = DMA2_REG;

// dma_callback_t

#define DMA_CHANNEL_COUNT 7

uint8_t dma_channels_busy[2][DMA_CHANNEL_COUNT];

void reset_flags_dma(DMA_Type *dma, DMA_Channel channel)
{
    if (dma == NULL)
        return;

    dma->IFCR = DMA_IFCR_CTEIFx(channel) | DMA_IFCR_CHTIFx(channel) | DMA_IFCR_CTCIFx(channel) | DMA_IFCR_CGIFx(channel);
}

void set_enabled_dma(DMA_Type *dma, DMA_Channel channel, uint8_t enable)
{
    if (dma == NULL)
        return;

    DMA_Channel_Settings *ch = &dma->channels[channel - 1];

    if (enable)
    {
        ch->CCR |= DMA_EN;
        while (!(ch->CCR & DMA_EN))
            ;
    }
    else
    {
        ch->CCR &= ~DMA_EN;
        while (ch->CCR & DMA_EN)
            ;
    }
}

void dma_init(DMA_Config *cfg)
{
    if (!cfg || !cfg->dma)
        return;

    DMA_Channel_Settings *ch = &cfg->dma->channels[cfg->channel - 1];

    // Выключаем канал перед настройкой
    ch->CCR &= ~DMA_EN;
    while (ch->CCR & DMA_EN)
        ;

    ch->CNDTR = 0;
    ch->CMAR = 0;
    ch->CPAR = cfg->peripheral_addr;

    if (cfg->direction == DMA_DIR_MEM_TO_PERIPH)
        ch->CCR |= DMA_DIR_MEMORY;
    else
        ch->CCR |= DMA_DIR_PERIPHERAL;

    ch->CCR |= cfg->mem_size;

    ch->CCR |= cfg->periph_size;

    if (cfg->inc_mem)
        ch->CCR |= DMA_MINC;

    if (cfg->inc_periph)
        ch->CCR |= DMA_PINC;

    if (cfg->circular)
        ch->CCR |= DMA_CIRC;
}

uint8_t is_dma_channel_busy(uint8_t dma_idx, DMA_Channel channel)
{
    return dma_channels_busy[dma_idx][channel];
}

void dma_send(DMA_Config *cfg, void *buffer, uint32_t length)
{
    if (!cfg || !cfg->dma)
        return;

    DMA_Channel_Settings *ch = &cfg->dma->channels[cfg->channel - 1];
    uint8_t dma_idx = (cfg->dma == DMA2) ? 1 : 0;

    while (is_dma_channel_busy(dma_idx, cfg->channel))
        ;

    ch->CCR &= ~DMA_EN;
    while (ch->CCR & DMA_EN)
        ;

    reset_flags_dma(cfg->dma, cfg->channel);

    ch->CMAR = (uint32_t)buffer;
    ch->CNDTR = length;

    dma_channels_busy[dma_idx][cfg->channel - 1] = 1;

    ch->CCR |= DMA_EN;
}

void DMA2_Channel7_IRQHandler()
{
    DMA_Channel_Settings *ch = &DMA2->channels[6];
    DMA_Type *dma = DMA2;

    if (dma->ISR & DMA_SR_CTCIFx(CHANNEL_7))
    {
        dma->IFCR |= DMA_IFCR_CTCIFx(CHANNEL_7);
        ch->CCR &= ~DMA_EN;
        dma_channels_busy[1][6] = 0;
    }

    if (dma->ISR & DMA_SR_CTEIFx(CHANNEL_7))
    {
        dma->IFCR |= DMA_IFCR_CTEIFx(CHANNEL_7);
        dma_channels_busy[1][6] = 0;
    }

    if (dma->ISR & DMA_SR_CHTIFx(CHANNEL_7))
        dma->IFCR |= DMA_IFCR_CHTIFx(CHANNEL_7);

    if (dma->ISR & DMA_SR_CGIFx(CHANNEL_7))
        dma->IFCR |= DMA_IFCR_CGIFx(CHANNEL_7);
}
