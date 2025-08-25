SET(LIBRARY_GRAPHICS driver_graphics)


add_library(${LIBRARY_GRAPHICS} STATIC
graphics.h graphics.c fonts.h fonts.c 
math_pixels.h math_pixels.c
)


target_include_directories(${LIBRARY_GRAPHICS}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})