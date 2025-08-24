SET(LIBRARY_CORE_NAME drivers_core)


add_library(${LIBRARY_CORE_NAME} STATIC
    memory_map.h
    utils.h utils.c
    flash.h
    gpio.h gpio.c
    rcc.h rcc.c
    led.h led.c
    timer.h timer.c
    spi.h spi.c
    i2c.h i2c.c
    usart.h usart.c
    dma.h dma.c
    IRQ.h IRQ.c
    sysmem.c
    board_pins.h
)

target_include_directories(${LIBRARY_CORE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})