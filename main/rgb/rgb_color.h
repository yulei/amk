/**
 * rgb_color.h
 *  rgb color definitions
 */

#pragma once

#include <stdint.h>

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

typedef rgb_t rgb_led_t;

rgb_t hsv_to_rgb(hsv_t hsv);

rgb_t pick_color(uint8_t hue);
