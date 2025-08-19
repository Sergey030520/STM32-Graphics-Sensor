#!/bin/bash

st-flash erase
st-flash write build/IPS_TFT_ST7789.bin 0x8000000