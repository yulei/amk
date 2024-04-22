/**
 * @file rgb_indicator.h
 */

#pragma once

#include <stdint.h>

enum rgb_indicator_type {
    RGB_INDICATOR_NUM_LOCK,
    RGB_INDICATOR_CAPS_LOCK,
    RGB_INDICATOR_SCROLL_LOCK,
    RGB_INDICATOR_COMPOSE,
    RGB_INDICATOR_KANA,
};


#define RGB_INDICATOR_STANDALONE 0xFF
struct rgb_indicator {
    uint8_t     index;
    uint8_t     type;
    uint8_t     config;
};

#ifdef RGB_INDICATOR_NUM
extern struct rgb_indicator g_rgb_indicators[RGB_INDICATOR_NUM];
#endif

void rgb_indicator_init(void);
void rgb_indicator_task(void);
void rgb_indicator_flush(void);
void rgb_indicator_uninit(void);
void rgb_indicator_prepare_sleep(void);

void rgb_indicator_get_led(uint8_t index, uint8_t* hue, uint8_t* sat, uint8_t* val, uint8_t* param);
void rgb_indicator_set_led(uint8_t index, uint8_t hue, uint8_t sat, uint8_t val, uint8_t param);
