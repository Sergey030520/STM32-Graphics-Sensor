SET(LIBRARY_OLED_ssd1306 driver_oled_ssd1306)


add_library(${LIBRARY_OLED_ssd1306} STATIC
oled_ssd1306.h oled_ssd1306.c
)

target_include_directories(${LIBRARY_OLED_ssd1306} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})