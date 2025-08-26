cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-none-eabi-gcc.cmake)

project(IPS_TFT_ST7789 C ASM)

set(EXECUTABLE ${PROJECT_NAME}.elf)
set(LINKER_FILE ${CMAKE_SOURCE_DIR}/conf_mem_stm32f108/STM32F103C8TX_FLASH.ld)

add_subdirectory(drivers)
add_subdirectory(logger)


set(ARM_FLAGS
        -mcpu=cortex-m3
        -mthumb
        -mfloat-abi=soft
        -fdata-sections
        -ffunction-sections
        -Wall
        -O0
        -g3
)


add_executable(${EXECUTABLE} 
    startup/startup_stm32f103c8tx.s
    main.c
    usart_board.h usart_board.c
    oled_board.h oled_board.c
    tft_board.h tft_board.c
    sensors.h sensors.c
    gfx.h gfx.c
)


target_compile_options(drivers_core PRIVATE ${ARM_FLAGS})
target_compile_options(logger PRIVATE ${ARM_FLAGS})
target_compile_options(driver_tft_st7789 PRIVATE ${ARM_FLAGS})
target_compile_options(driver_oled_ssd1306 PRIVATE ${ARM_FLAGS})
target_compile_options(driver_graphics PRIVATE ${ARM_FLAGS})
target_compile_options(driver_sensor_sc7a20h PRIVATE ${ARM_FLAGS})
target_compile_options(${EXECUTABLE} PRIVATE ${ARM_FLAGS})

target_link_libraries(${EXECUTABLE} PRIVATE
        drivers_core
        logger
        driver_tft_st7789
        driver_oled_ssd1306
        driver_graphics
        driver_sensor_sc7a20h
        m
)

target_link_options(${EXECUTABLE} PRIVATE
        -T${LINKER_FILE}
        -mcpu=cortex-m3
        -mthumb
        -mfloat-abi=soft
        -specs=nano.specs
        -lc
        -lm
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        )

add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE}
        )

add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O srec --srec-len=64 ${EXECUTABLE} ${PROJECT_NAME}.s19
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
        )