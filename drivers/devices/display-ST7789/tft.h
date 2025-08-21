#include <stdint.h>
#include "math_pixels.h"

#pragma once

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 240

typedef struct
{
    uint16_t Blue : 5;
    uint16_t Green : 6;
    uint16_t Red : 5;
} RGB_Type; // 5-6-5

typedef struct
{
    uint16_t x0;
    uint16_t y0;
    uint16_t x1;
    uint16_t y1;
} Coordinate;

typedef void (*pin_ctrl_t)(int state);
typedef int (*spi_func_t)(uint8_t *data, uint32_t size);
typedef void (*delay_timer_t)(uint32_t ms);
typedef void (*brightness_func_t)(uint8_t volume);

typedef struct
{
    // Управление пинами
    pin_ctrl_t set_dc;  // Data/Command
    pin_ctrl_t set_res; // Reset
    pin_ctrl_t set_blk; // Backlight

    spi_func_t spi_send;
    spi_func_t spi_recv;

    delay_timer_t delay_ms;

    brightness_func_t set_brightness;
} TFT_Interface_t;

#define CMD_SWRESET 0x01   // software operation
#define CMD_RDID 0x04      // Read display ID
#define CMD_RDDST 0x09     // read display status
#define CMD_RDDPM 0x0A     // read display power
#define CMD_RDDMADCTL 0x0B // read display
#define CMD_RDDCOLMOD 0x0C // read display pixel
#define CMD_RDDDIM 0x0D    // read display image
#define CMD_RDDSM 0x0E     // read display signal

#define CMD_RDDSDR 0x0F // read display self-diagnostic result
#define CMD_SLPIN 0x10  // Sleep in
#define CMD_SLPOUT 0x11 // Sleep out
#define CMD_PTLON 0x12  // partial mode on
#define CMD_NORON 0x13  // partial off(Normal)

#define CMD_INVOFF 0x20 // Display inversion off
#define CMD_INVON 0x21  // Display inversion on
#define CMD_GAMSET 0x26 // Display inversion on

#define CMD_DISPOFF 0x28 // Display off
#define CMD_DISPON 0x29  // Display on
#define CMD_CASET 0x2A   // Column address set
#define CMD_RASET 0x2B   // Row address set
#define CMD_RAMWR 0x2C   // Memory write
#define CMD_RAMRD 0x2E   // Memory read

#define CMD_PTLAR 0x30    // Partial sart/end address set
#define CMD_VSCRDEF 0x33  // Vertical scrolling defenition
#define CMD_TEOFF 0x34    // Tearing effect line off
#define CMD_TEON 0x35     // Tearing effect line on
#define CMD_MADCTL 0x36   // Memory data access control
#define CMD_VSCRSADD 0x37 // Vertical scrolling start address
#define CMD_IDMOFF 0x38   // Idle mode off
#define CMD_IDMON 0x39    // Idle mode on

#define CMD_COLMOD 0x3A   // Interface pixel format
#define CMD_RAMWRC 0x3C   // Memory write continue
#define CMD_RAMRDC 0x3E   // Memory read continue
#define CMD_TESCAN 0x44   // Set tear scanline
#define CMD_RDTESCAN 0x45 // Get scanline
#define CMD_WRDISBV 0x51  // Write display brightness
#define CMD_RDDISBV 0x52  // Read display brightness value
#define CMD_WRCTRLD 0x53  // Write CTRL display
#define CMD_RDCTRLD 0x54  // Read CTRL value display

#define CMD_WRCACE 0x55   // Write content adaptive brightness control and Color enhancemnet
#define CMD_RDCABC 0x56   // Read content adaptive brightness control
#define CMD_WRCABCMB 0x5E // Write CABC minimum brightness
#define CMD_RDCABCMB 0x5F // Read CABC minimum brightness
#define CMD_RDABCSDR 0x68 // Read Automatic brightness control Self-Diagnostic result
#define CMD_RDID1 0xDA    // Read ID1
#define CMD_RDID2 0xDB    // Read ID2
#define CMD_RDID3 0xDC    // Read ID3

#define CMD_RAMCTRL 0xB0  // Ram control
#define CMD_RGBCTRL 0xB1  // RGB Control
#define CMD_PORCTRL 0xB2  // Porch control
#define CMD_FRCCTRL1 0xB3 // Frame Rate Control in Normal Mode 1
#define CMD_PARCTRL 0xB5  // Partial mode control
#define CMD_GCTRL 0xB7    // Gate control
#define CMD_GTADJ 0xB8    // Gate on timing adjustment
#define CMD_DGMEN 0xBA    // Digital Game enable

#define CMD_VCOMS 0xBB    // VCOMS settings
#define CMD_LCMCTRL 0xC0  // LCM Control
#define CMD_IDSET 0xC1    // ID settings
#define CMD_VDVVRHEN 0xC2 // VDV and VRH Command Enable
#define CMD_VRHS 0xC3     // VRH set
#define CMD_VDVS 0xC4     // VDV set
#define CMD_VCMOFSET 0xC5 // VCOM offset set
#define CMD_FRCTRL2 0xC6  // Frame Rate Control 2

#define CMD_PWCTRL1 0xD0   // Power Control 1
#define CMD_PVGAMCTRL 0xE0 // Positive Voltage Gamma Control
#define CMD_NVGAMCTRL 0xE1 // Negative Voltage Gamma Control

// For cmd COLMOD
#define BIT_PIXEl_12 (0b011 << 0)
#define BIT_PIXEl_16 (0b101 << 0)
#define BIT_PIXEl_168 (0b110 << 0)
#define BIT_PIXEl_16M_TRUNC (0b111 << 0)

#define COLOR_MODE_65K (0b101 << 4)
#define COLOR_MODE_262K (0b110 << 4)

// MADCLT

#define MADCLT_MY (0x1 << 7)
#define MADCLT_MX (0x1 << 6)
#define MADCLT_MV (0x1 << 5)
#define MADCLT_ML (0x1 << 4)
#define MADCLT_BGR (0x1 << 3)
#define MADCLT_MH (0x1 << 2)

#define RGB565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

#define VIOLET_COLOR RGB565(255, 0, 255)
#define RED_COLOR RGB565(255, 0, 0)
#define GREEN_COLOR RGB565(0, 255, 0)
#define BLUE_COLOR RGB565(0, 0, 255)
#define YELLOW_COLOR RGB565(255, 0, 255)

// brightness
#define BCTRL_ON (0x1 << 5)
#define DISPLAY_DIMMING (0x1 << 3)
#define BLACKLIGHT_CONTROL (0x1 << 2)

void send_command(uint8_t command);
void send_data_array_command(uint8_t *data, uint32_t size);
void send_data_command(uint8_t data);

void set_brightness(uint8_t volume);

void set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

// simple objects
void draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void draw_circle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color);
void draw_triangle(Point point1, Point point2, Point point3, uint16_t color);
void draw_line_bresenham(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

void fill(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY, uint16_t color);
void fill_color_display(uint16_t color);

void clear_screen();

void st7789_init(TFT_Interface_t *tft_interface);

// https://blog.embeddedexpert.io/?p=1215
