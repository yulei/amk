/**
 * rgb_matrix.h
 *      rgb matrix definition
 */

#pragma once

#include <stdbool>
#include <stdint.h>

#includr "rgb_driver.h"

#ifndef RGB_MATRIX_LED_NUM
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
    rgb_matrix_led_t leds[RGB_MATRIX_LED_NUM]; // led definition depends on the specific driver
    rgb_matrix_led_attribute_t attributes[RGB_MATRIX_LED_NUM]; // led common attribute
} rgb_matrix_leds_t;

extern rgb_matrix_leds_t g_rgb_matrix_leds;

void rgb_matrix_init(rgb_driver_t *driver);
bool rgb_matrix_enabled(void);
void rgb_matrix_inc_hue(void);
void rgb_matrix_dec_hue(void);
void rgb_matrix_inc_sat(void);
void rgb_matrix_dec_sat(void);
void rgb_matrix_inc_val(void);
void rgb_matrix_dec_val(void);
void rgb_matrix_inc_speed(void);
void rgb_matrix_dec_speed(void);
void rgb_matrix_inc_mode(void);
void rgb_matrix_dec_mode(void);
void rgb_matrix_toggle(void);
void rgb_matrix_task(void);