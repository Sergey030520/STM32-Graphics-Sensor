#include "utils.h"
#include <stdint.h>
#include "rcc.h"


int wait_for_flags(volatile uint32_t *reg, uint32_t flags, uint32_t timeout)
{
    while (timeout--)
    {
        if ((*reg & flags) == flags)
            return 0; 
    }
    return -1;
}