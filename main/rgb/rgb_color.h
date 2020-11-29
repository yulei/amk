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

#ifdef RGB_MATRIX_ENABLE

#ifdef RGB_MATRIX_LED_NUM
#error "RGB_MATRIX_LED_NUM must be defined"
#endif

typedef struct {
    uint8_t driver;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_matrix_led_t;

typdef struct {
    uint8_t x;  // postion x
    uint8_t y;  // position y
    uint8_t a;  // attribute
    uint8_t r;  // mapped key matrix row
    uint8_t c;  // mapped key matrix col
} rgb_matrix_led_attribute_t;

typedef struct
{
    rgb_matrix_led_t leds[RGB_MATRIX_LED_NUM];                  // led definition depends on the specific driver
    rgb_matrix_led_attribute_t attributes[RGB_MATRIX_LED_NUM];  // led common attribute
} rgb_matrix_leds_t;

extern rgb_matrix_leds_t g_rgb_matrix_leds;
#endif
