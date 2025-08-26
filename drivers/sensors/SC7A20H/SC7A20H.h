#pragma once

#include <stdint.h>

#define WHO_AM_I_SC7A20H 0x0f        // rw
#define CTRL_REG0_SC7A20H 0x1f       // rw
#define CTRL_REG1_SC7A20H 0x20       // rw
#define CTRL_REG2_SC7A20H 0x21       // rw
#define CTRL_REG3_SC7A20H 0x22       // rw
#define CTRL_REG4_SC7A20H 0x23       // rw
#define CTRL_REG5_SC7A20H 0x24       // rw
#define CTRL_REG6_SC7A20H 0x25       // rw
#define DRDY_STATUS_REG_SC7A20H 0x27 // r
#define OUT_X_L_SC7A20H 0x28         // r
#define OUT_X_H_SC7A20H 0x29         // r
#define OUT_Y_L_SC7A20H 0x2A         // r
#define OUT_Y_H_SC7A20H 0x2B         // r
#define OUT_Z_L_SC7A20H 0x2C         // r
#define OUT_Z_H_SC7A20H 0x2D         // r
#define FIFO_CTRL_REG_SC7A20H 0x2E   // rw
#define FIFO_SRC_REG_SC7A20H 0x2F    // r
#define AOI1_CFG_SC7A20H 0x30        // rw
#define AOI1_SRC_SC7A20H 0x31        // r
#define AOI1_THS_SC7A20H 0x32        // rw
#define AOI1_DURATION_SC7A20H 0x33   // rw
#define AOI2_CFG_SC7A20H 0x34        // rw
#define AOI2_SRC_SC7A20H 0x35        // r
#define AOI2_THS_SC7A20H 0x36        // rw
#define AOI2_DURATION_SC7A20H 0x37   // rw
#define CLICK_CRTL_REG_SC7A20H 0x38  // rw
#define CLICK_SRC_SC7A20H 0x39       // r
#define CLICK_COEFF1_SC7A20H 0x3A    // rw
#define CLICK_COEFF2_SC7A20H 0x3B    // rw
#define CLICK_COEFF3_SC7A20H 0x3C    // rw
#define CLICK_COEFF4_SC7A20H 0x3D    // rw
#define OUT_X_L_SC7A20H 0x61         // r
#define OUT_X_H_SC7A20H 0x62         // r
#define OUT_Y_L_SC7A20H 0x63         // r
#define OUT_Y_H_SC7A20H 0x64         // r
#define OUT_Z_L_SC7A20H 0x65         // r
#define OUT_Z_H_SC7A20H 0x66         // r
#define SOFT_RESET_SC7A20H 0x68      // rw
#define FIFO_DATA_SC7A20H 0x69       // r
#define I2C_CTRL_SC7A20H 0x6F        // rw


#define SC7A20H_ADDRES_I2C 0x18


typedef struct
{
    int16_t X;
    int16_t Y;
    int16_t Z;
} SC7A20H_Data_t;


typedef int (*SC7A20H_WriteFunc_t)(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len);
typedef int (*SC7A20H_ReadFunc_t)(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len);

typedef struct
{
    SC7A20H_WriteFunc_t write; 
    SC7A20H_ReadFunc_t read;   
    SC7A20H_Data_t last_data;  
    uint8_t sensitivity;       // Настройка чувствительности
    uint8_t odr;               // Output Data Rate, Гц
    int is_initialized;       // Флаг инициализации
    uint8_t dev_addr; 
} SC7A20H_Handle_t;

int init_sc7a20h(SC7A20H_Handle_t *handle);
int read_xyz_sc7a20h(SC7A20H_Handle_t *handle, SC7A20H_Data_t *vector);