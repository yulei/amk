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
    uint8_t x;  // postion x
    uint8_t y;  // position y
    uint8_t a;  // attribute
    uint8_t r;  // mapped key matrix row
    uint8_t c;  // mapped key matrix col
} rgb_led_attr_t;

typedef struct {
    uint8_t type;
    uint8_t addr;
    uint8_t index;
    uint8_t led_start;
    uint8_t led_num;
} rgb_device_t;

typedef struct {
    uint8_t enable;
    uint8_t mode;
    uint8_t speed;
    uint8_t hue;
    uint8_t sat;
    uint8_t val;
    uint8_t type;
    uint8_t index;
    void*   data;
} rgb_config_t;

typedef struct {
    uint8_t config;
    uint8_t led_start;
    uint8_t led_num;
} rgb_param_t;

enum {
    RGB_EFFECT_ENABLE,
    RGB_EFFECT_MODE,
    RGB_EFFECT_SPEED,
    RGB_EFFECT_HUE,
    RGB_EFFECT_SAT,
    RGB_EFFECT_VAL,
};

enum {
    RGB_EFFECT_LINEAR,
    RGB_EFFECT_INDICATOR,
    RGB_EFFECT_MATRIX,
};

#define ENABLE_DEFAULT  1
#define MODE_DEFAULT    0

#define SPEED_MIN 1
#define SPEED_MAX 15
#define SPEED_DEFAULT 7
#define SPEED_STEP  1

#define HUE_MIN 0
#define HUE_MAX 255
#define HUE_DEFAULT HUE_MIN
#define HUE_STEP 8

#define SAT_MIN 0
#define SAT_MAX 255
#define SAT_DEFAULT SAT_MAX
#define SAT_STEP 8

#define VAL_MIN 0
#define VAL_MAX 255
#define VAL_DEFAULT VAL_MAX
#define VAL_STEP 8

typedef void* rgb_effect_t;
typedef void (*rgb_effect_fun_t)(rgb_effect_t*);

extern rgb_led_t g_rgb_leds[];
extern rgb_led_attr_t g_rgb_led_attrs[];

extern rgb_device_t g_rgb_devices[];
extern rgb_config_t g_rgb_configs[];

rgb_t hsv_to_rgb(hsv_t hsv);
rgb_t pick_color(uint8_t hue);
uint8_t pick_hue(void);

#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))