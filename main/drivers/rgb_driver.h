/**
 * rgb_driver.h
 *  rgb led driver implementation
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef void (*rgb_driver_init)(void);
typedef void (*rgb_driver_uninit)(void);
typedef void (*rgb_driver_set_color)(uint32_t index, uint8_t hue, uint8_t sat, uint8_t val);
typedef void (*rgb_driver_set_color_all)(uint8_t hue, uint8_t sat, uint8_t val);
typedef void (*rgb_driver_flush)(void);

typedef struct {
    rgb_driver_init init;
    rgb_driver_uninit uninit;
    rgb_driver_set_color set_color;
    rgb_driver_set_color_all set_color_all;
    rgb_driver_flush flush;
} rgb_driver_t;

typedef enum {
    RGB_DRIVER_WS2812,
    RGB_DRIVER_AW9523B,
    RGB_DRIVER_AW9106B,
    RGB_DRIVER_IS31FL3731,
    RGB_DRIVER_IS31FL3733,
} RGB_DRIVER_TYPE;

bool rgb_driver_available(RGB_DRIVER_TYPE type);
rgb_driver_t* rgb_driver_create(RGB_DRIVER_TYPE type);
void rgb_driver_destroy(rgb_driver_t* driver);
