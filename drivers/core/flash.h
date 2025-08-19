#pragma once 

#include <stdint.h>


typedef struct{
    uint32_t ACR;
    uint32_t KEYR;
    uint32_t OPTKEYR;
    uint32_t SR;
    uint32_t CR;
    uint32_t AR;
    uint32_t reserved;
    uint32_t OBR;
    uint32_t WRPR;
} FLASH_Type;

typedef enum {
    LATENCY_ZERO = 0x0,
    LATENCY_ONE = 0x1,
    LATENCY_TWO = 0x2
}FLASH_Latency;

#define FLASH_ACR_LATENCY_MASK (0x7 << 0) 
#define FLASH_ACR_LATENCY(latency) (latency << 0)

