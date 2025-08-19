#include <stdint.h>
#include <string.h>
#include <stdarg.h>


#pragma once


#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

typedef int (*DataTransferFunc)(const char* data, int length);

typedef enum
{
    LEVEL_DEBUG = 0,
    LEVEL_INFO = 1,
    LEVEL_WARNING,
    LEVEL_ERROR
} LogLevel;


#pragma pack(push, 1)
typedef struct
{
    uint16_t length;
    uint8_t level;
    char buffer[4096];
} LogMessage;
#pragma pack(pop)

typedef enum
{
    GROUP_1BYTE = 1,
    GROUP_2BYTE = 2,
    GROUP_4BYTE = 4
} LogGroupBytes;

extern LogLevel current_level;

void stm_init_log(DataTransferFunc func);
void stm_log(LogLevel level, uint16_t err_code, const char *file, int line, const char *format, ...);
void vstm_log(LogLevel level, uint16_t err_code, const char *file, int line, const char *format, va_list args);
void stm_log_hex(LogLevel level, uint16_t err_code, const char *file, int line, uint8_t *data, int size, LogGroupBytes group);


#define LOG_INFO_HEX(array, length, group_bytes) stm_log_hex(LEVEL_INFO, 0, __FILENAME__, __LINE__, array, length, group_bytes)
#define LOG_DEBUG_HEX(array, length, group_bytes) stm_log_hex(LEVEL_DEBUG, 0, __FILENAME__, __LINE__, array, length, group_bytes)
#define LOG_WARNING_HEX(array, length, group_bytes) stm_log_hex(LEVEL_WARNING, 0, __FILENAME__, __LINE__, array, length, group_bytes)
#define LOG_ERROR_HEX(err_code, array, length, group_bytes) stm_log_hex(LEVEL_ERROR, err_code, __FILENAME__, __LINE__, array, length, group_bytes)


#define LOG_INFO(format, ...) stm_log(LEVEL_INFO, 0, __FILENAME__, __LINE__, format,  ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) stm_log(LEVEL_DEBUG, 0, __FILENAME__, __LINE__,  format,  ##__VA_ARGS__)
#define LOG_WARNING(format, ...) stm_log(LEVEL_WARNING, 0, __FILENAME__, __LINE__,  format,  ##__VA_ARGS__)
#define LOG_ERROR(err_code, format, ...) stm_log(LEVEL_ERROR, err_code, __FILENAME__, __LINE__,  format,  ##__VA_ARGS__)

