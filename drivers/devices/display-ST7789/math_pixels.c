#include "math_pixels.h"



Point findMinPoint(Point *points, int16_t size)
{
    Point point = {UINT16_MAX, UINT16_MAX};
    for (int16_t i = 0; i < size; ++i)
    {
        if (points[i].x < point.x)
        {
            point.x = points[i].x;
        }
        if (points[i].y < point.y)
        {
            point.y = points[i].y;
        }
    }
    return point;
}

Point findMaxPoint(Point *points, int16_t size)
{
    Point point = {0, 0};
    for (int16_t i = 0; i < size; ++i)
    {
        if (points[i].x > point.x)
        {
            point.x = points[i].x;
        }
        if (points[i].y > point.y)
        {
            point.y = points[i].y;
        }
    }
    return point;
}