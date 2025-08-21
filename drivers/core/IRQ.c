#include "IRQ.h"
#include "memory_map.h"


NVIC_Type *NVIC = NVIC_REG;


static inline void __enable_irq(void)
{
    asm volatile("cpsie i" : : : "memory");
}

void init_irq()
{
    // NVIC->ISERx[0] = NVIC_DMA1_CHANNEL3; // NVIC_DMA1_CHANNEL3; // DMA1|_Channel_5
    // NVIC->ISERx[1] = NVIC_SPI1;          // | NVIC_SPI1;                   //| NVIC_USART1;
    // enable_irq_nvic(DMA1_Channel1_IRQ, 6);
    // enable_irq_nvic(DMA1_Channel2_IRQ, 6);
    enable_irq_nvic(DMA1_Channel3_IRQ, 6);
    enable_irq_nvic(DMA1_Channel4_IRQ, 6);
    // enable_irq_nvic(DMA1_Channel5_IRQ, 6);
    // enable_irq_nvic(DMA1_Channel6_IRQ, 6);
    // enable_irq_nvic(DMA1_Channel7_IRQ, 6);
    // enable_irq_nvic(SPI1_IRQ, 6);

    __enable_irq();
}

void enable_irq_nvic(uint8_t irqn, uint8_t priority)
{
    // Устанавливаем приоритет
    NVIC->IPRx[irqn] = (priority & 0xF) << 4;

    // Разрешаем прерывани
    NVIC->ISERx[irqn / 32] |= (1UL << (irqn % 32));
}
void disable_irq_nvic(uint8_t irqn)
{
    NVIC->ICERx[irqn / 32] = (1UL << (irqn % 32));
}