/**
 * @file rgb_driver.h
 * @brief rgb led driver implementation
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "rgb_common.h"

typedef struct s_rgb_driver rgb_driver_t;

typedef void (*rd_init)(rgb_driver_t *driver);
typedef void (*rd_uninit)(rgb_driver_t* driver);
typedef void (*rd_set_color)(rgb_driver_t* driver, uint32_t index, uint8_t hue, uint8_t sat, uint8_t val);
typedef void (*rd_set_color_all)(rgb_driver_t *driver, uint8_t hue, uint8_t sat, uint8_t val);
typedef void (*rd_flush)(rgb_driver_t *driver);

struct s_rgb_driver{
    rd_init init;
    rd_uninit uninit;
    rd_set_color set_color;
    rd_set_color_all set_color_all;
    rd_flush flush;
    rgb_device_t *device;
    void *data;
};

typedef enum {
    RGB_DRIVER_WS2812,
    RGB_DRIVER_AW9523B,
    RGB_DRIVER_AW9106B,
    RGB_DRIVER_IS31FL3731,
    RGB_DRIVER_IS31FL3733,
    RGB_DRIVER_IS31FL3236,
} rgb_driver_type_t;

bool rgb_driver_available(rgb_driver_type_t type);
void rgb_driver_init(void);
rgb_driver_t *rgb_driver_get(uint8_t index);
//rgb_driver_t* rgb_driver_create(rgb_device_t *device);
void rgb_driver_destroy(rgb_driver_t* driver);
