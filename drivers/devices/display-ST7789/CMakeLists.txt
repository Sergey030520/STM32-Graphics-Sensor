SET(LIBRARY_TFT7789 driver_tft_st7789)


add_library(${LIBRARY_TFT7789} STATIC
    tft.h tft.c
)

target_include_directories(${LIBRARY_TFT7789} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/logger)