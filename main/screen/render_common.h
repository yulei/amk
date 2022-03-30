/**
 * @file render_common.h
 * 
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "amk_gpio.h"

enum {
    SPI_LCD_ST7735,
    SPI_LCD_ST7789,
    SPI_LCD_RM67160,
    SPI_LCD_SSD1357,
};

enum {
    ANM_FILE,
    AUX_FILE,
    SML_FILE,
};

enum {
    ANIM_DISPLAY,
    AUDIO_DISPLAY,
//    INFO_DISPLAY,
//    KEYBOARD_DISPLAY,
};

typedef struct {
    pin_t       ctrl;   // power chip control pin, exist on amoled
    pin_t       power;
    uint8_t     power_on;
    pin_t       reset;
    pin_t       cs;
    pin_t       dc;
    pin_t       sda;
    pin_t       scl;
    uint8_t     type;
} screen_driver_param_t;

typedef struct {
    uint32_t    x;  // x origin
    uint32_t    y;  // y origin
    uint32_t    width;
    uint32_t    height;
    uint8_t     driver; // driver index
} screen_param_t;

typedef struct {
    uint8_t     type;
    uint8_t     screen; // screen index
    uint32_t    width;
    uint32_t    height;
    uint8_t     file_type;
    uint8_t     audio_format;
    uint8_t     enabled;
} display_param_t;

typedef struct screen_driver_s screen_driver_t;

typedef struct screen_driver_s {
    void (*init)(screen_driver_t *driver);
    void (*uninit)(screen_driver_t *driver);
    void (*clear)(screen_driver_t *driver, void *data);
    void (*fill)(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
    void (*fill_async)(screen_driver_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
    void (*plot)(screen_driver_t *driver, uint32_t x, uint32_t y, uint16_t color);
    bool (*ready)(screen_driver_t *driver);
    void (*release)(screen_driver_t *driver);
    void *data;
} screen_driver_t;

typedef struct screen_s screen_t;
typedef struct screen_s {
    bool (*init)(screen_t *screen, screen_driver_t * driver);
    void (*uninit)(screen_t *screen);
    void (*fill_rect)(screen_t *screen, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, uint32_t size);
    void (*fill_rect_async)(screen_t *screen, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, uint32_t size);
    uint8_t* (*get_buffer)(screen_t *screen);
    bool (*ready)(screen_t *screen);
    bool (*test)(screen_t *screen);
    void *data;
} screen_t;

typedef struct display_s display_t;
typedef struct display_s {
    bool (*init)(display_t *display, screen_t *screen);
    void (*uninit)(display_t *display);
    void (*task)(display_t *display);
    void (*set_enable)(display_t *display, bool enable);
    bool (*is_enabled)(display_t *display);
    void *data;
} display_t;

uint8_t* render_buffer_allocate(uint32_t size);