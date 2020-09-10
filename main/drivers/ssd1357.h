/**
 * ssd1357.h
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "gpio_pin.h"

typedef struct {
    int32_t x;
    int32_t y;
} point_t;

typedef struct {
    point_t pos;
    uint32_t width;
    uint32_t height;
} rect_t;

typedef struct {
    pin_t reset;
    pin_t cs;
    pin_t dc;
} ssd1357_t;

bool ssd1357_init(ssd1357_t* driver);
void ssd1357_fill_rect(ssd1357_t* driver, rect_t rect, const void *data, size_t size);
void ssd1357_uninit(ssd1357_t* driver);
