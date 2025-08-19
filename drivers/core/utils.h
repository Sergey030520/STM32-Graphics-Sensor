#pragma once

#include <stdint.h>


int wait_for_flags(volatile uint32_t *reg, uint32_t flags, uint32_t timeout);