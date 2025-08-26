#pragma once


#include <stdint.h>

typedef struct
{
    int16_t X;
    int16_t Y;
    int16_t Z;
} Vector;


void sensors_init();
int get_data_sensor_sc7a20h(Vector *data);