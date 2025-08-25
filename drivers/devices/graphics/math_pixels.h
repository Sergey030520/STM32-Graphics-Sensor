#include <stdint.h>

#pragma once

typedef struct
{
    float x;
    float y;
} Point;


Point findMinPoint(Point *points, int16_t size);
Point findMaxPoint(Point *points, int16_t size);