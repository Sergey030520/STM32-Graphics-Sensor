set(LIBRARY_LOGGER_NAME logger)

add_library(${LIBRARY_LOGGER_NAME} STATIC
    log.h log.c
)

target_include_directories(${LIBRARY_LOGGER_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
