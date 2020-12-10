/**
 * @file is31.h
 *  common definitions for the is31 series led control
 */

#pragma once

#include <stdint.h>

typedef struct {
    uint8_t     addr;
    uint8_t     led_num;
    void        *user;       
} is31_t;

#ifndef IS31_DRIVER_NUM
#define IS31_DRIVER_NUM     1
#endif

#define IS31_TIMEOUT        100

typedef struct {
    uint8_t driver;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} is31_led_t;

typedef struct {
    uint8_t x;  // postion x
    uint8_t y;  // position y
    uint8_t a;  // attribute
    uint8_t r;  // mapped key matrix row
    uint8_t c;  // mapped key matrix col
} is31_led_attribute_t;

#ifdef RGB_MATRIX_ENABLE
#ifndef RGB_MATRIX_LED_NUM
#error "RGB_MATRIX_LED_NUM must be defined"
#endif

typedef struct
{
    is31_led_t leds[RGB_MATRIX_LED_NUM];                  // led definition depends on the specific driver
    is31_led_attribute_t attributes[RGB_MATRIX_LED_NUM];  // led common attribute
} rgb_matrix_t;

extern rgb_matrix_t g_rgb_matrix;
#endif