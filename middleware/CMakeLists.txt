
add_library(middleware
    tft_board.h tft_board.c
    oled_board.h oled_board.c
    usart_board.h usart_board.c
    gfx.h gfx.c
    sensors.h sensors.c
)

target_include_directories(middleware PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/middleware
    ${CMAKE_SOURCE_DIR}/drivers_core
    ${CMAKE_SOURCE_DIR}/logger
    ${CMAKE_SOURCE_DIR}/driver_tft_st7789
    ${CMAKE_SOURCE_DIR}/driver_oled_ssd1306
    ${CMAKE_SOURCE_DIR}/driver_graphics
    ${CMAKE_SOURCE_DIR}/driver_sensor_sc7a20h
)

target_link_libraries(middleware PRIVATE
    drivers_core
    logger
    driver_tft_st7789
    driver_oled_ssd1306
    driver_graphics
    driver_sensor_sc7a20h
)