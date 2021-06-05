/**
 * @file is31.h
 *  common definitions for the is31 series led control
 */

#pragma once

#include <stdint.h>
#include "rgb_led.h"

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
    uint8_t x;  // postion x
    uint8_t y;  // position y
    uint8_t a;  // attribute
    uint8_t r;  // mapped key matrix row
    uint8_t c;  // mapped key matrix col
} is31_led_attribute_t;

extern rgb_led_t g_is31_leds[];

#ifdef RGB_MATRIX_ENABLE
#ifndef RGB_MATRIX_LED_NUM
#error "RGB_MATRIX_LED_NUM must be defined"
#endif

typedef struct
{
    is31_led_attribute_t attributes[RGB_MATRIX_LED_NUM];  // led common attribute
} rgb_matrix_t;

extern rgb_matrix_t g_rgb_matrix;
#endif