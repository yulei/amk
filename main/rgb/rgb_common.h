/**
 * rgb_common.h
 *  rgb common definitions
 */

#pragma once

#include <stdint.h>
#include "gpio_pin.h"

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_t;

typedef struct {
    uint8_t h;
    uint8_t s;
    uint8_t v;
} hsv_t;

typedef struct {
    uint8_t driver;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_led_t;

typedef struct {
    uint8_t type;
    uint8_t addr;
    uint8_t index;
    uint8_t led_start;
    uint8_t led_count;
} rgb_device_t;

extern rgb_led_t g_rgb_leds[RGB_LED_NUM];
extern rgb_device_t g_rgb_devices[RGB_DEVICE_NUM];

rgb_t hsv_to_rgb(hsv_t hsv);
rgb_t pick_color(uint8_t hue);
