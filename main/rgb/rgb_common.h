/**
 * @file rgb_common.h
 * @brief rgb common definitions
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

typedef struct {
    uint8_t enable;
    uint8_t mode;
    uint8_t speed;
    uint8_t hue;
    uint8_t sat;
    uint8_t val;
} rgb_config_t;

typedef enum {
    RGB_EFFECT_ENABLE,
    RGB_EFFECT_MODE,
    RGB_EFFECT_SPEED,
    RGB_EFFECT_HUE,
    RGB_EFFECT_SAT,
    RGB_EFFECT_VAL,
} rgb_config_param_t;

typedef void* rgb_effect_t;
typedef void (*rgb_effect_fun_t)(rgb_effect_t*);

extern rgb_led_t g_rgb_leds[];
extern rgb_device_t g_rgb_devices[];
extern rgb_config_t g_rgb_configs[];

rgb_t hsv_to_rgb(hsv_t hsv);
rgb_t pick_color(uint8_t hue);
