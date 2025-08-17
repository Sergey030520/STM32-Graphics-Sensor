#include <stdio.h>
#include "configuration.h"
#include "rcc.h"
#include "led.h"
#include "usart.h"
#include "dma.h"
#include "spi.h"
#include "tft.h"
#include "timer.h"
#include <string.h>

NVIC_Type *NVIC = NVIC_REG;

static inline void __enable_irq(void)
{
    asm volatile("cpsie i" : : : "memory");
}

void init_irq()
{
    NVIC->ISERx[0] = NVIC_DMA1_CHANNEL3; // NVIC_DMA1_CHANNEL3; // DMA1|_Channel_5
    NVIC->ISERx[1] = NVIC_SPI1;          // | NVIC_SPI1;                   //| NVIC_USART1;
    __enable_irq();
}

int main()
{
    init_rcc();
    init_led();
    init_uart();
    init_spi_master();
    init_irq();

    st7789_init();

    //
    // 63 g, 31 r, 31 - b
    // uint16_t color = 0xF8;

    fill_color_display(RGB565(0, 0, 255));
    // fill(0, 0, 239, 239, RGB565(0, 255, 0));



    // draw_line(160,50, 0, 67, RGB565(255, 0, 0));
    // draw_line(160,160, 200, 160, RGB565(0, 255, 0));
    
    // draw_rect(200, 200, 20, 20,  RGB565(255, 255, 0));
    // draw_rect(140, 140, 50, 50,  RGB565(0, 255, 255));
    // draw_rect(0, 0, 70, 70,  RGB565(0, 255, 255));

    Point point1 = {60, 20}, point2 = {30, 60}, 
    point3 = {90, 60};
    
    draw_triangle(point1, point2, point3, RGB565(255, 0, 0));
    draw_circle(140, 140, 30, RGB565(0,0, 255));
    // draw_line_bresenham(200, 200, 106, 203, RGB565(0,0,255));
    // set_brightness(90);
    // // fill();

    // delay_timer(2000);
    
    // set_brightness(200);

    // delay_timer(2000);
    
    // set_brightness(10);

    // delay_timer(2000);

    set_brightness(30);


    while (1)
    {
        delay_timer(2000);
        return 0;
    }
}