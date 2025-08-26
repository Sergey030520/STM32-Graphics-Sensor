#include "SC7A20H.h"




int init_sc7a20h(SC7A20H_Handle_t *handle)
{
    if (!handle) return -1;

    uint8_t id;
    int status = handle->read(handle->dev_addr, WHO_AM_I_SC7A20H, &id, sizeof(id));
    if (status != 0) {
        handle->is_initialized = 0;
        return -2;
    }

    if (id != 0x11) {
        handle->is_initialized = 0;
        return -3;
    }
    uint8_t value = 0x47;
    status = handle->write(handle->dev_addr, CTRL_REG1_SC7A20H, &value, 1); 
    if (status != 0) return -4;

    value = 0x00;
    status = handle->write(handle->dev_addr, CTRL_REG4_SC7A20H, &value, 1); 
    if (status != 0) return -5;

    handle->is_initialized = 1;
    return 0;
}

int read_xyz_sc7a20h(SC7A20H_Handle_t *handle, SC7A20H_Data_t *data)
{
    if (!handle || !data) return -1;
    if (!handle->is_initialized) return -2;

    uint8_t buffer[6];
    int status = handle->read(handle->dev_addr, OUT_X_L_SC7A20H | 0x80, buffer, 6); 
    if (status != 0) return -3;

    data->X = (int16_t)((buffer[1] << 8) | buffer[0]);
    data->Y = (int16_t)((buffer[3] << 8) | buffer[2]);
    data->Z = (int16_t)((buffer[5] << 8) | buffer[4]);

    handle->last_data = *data;
    return 0;
}

