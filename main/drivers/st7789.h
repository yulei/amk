
/**
 * @file st7789.h
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "amk_gpio.h"

typedef struct {
#ifdef SCREEN_POWER_PIN
    pin_t power;
#endif
    pin_t reset;
    pin_t cs;
    pin_t dc;
} st7789_t;

void st7789_init(st7789_t *driver);
void st7789_fill_rect(st7789_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
void st7789_fill_rect_async(st7789_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
bool st7789_fill_ready(st7789_t *driver);
void st7789_release(st7789_t *driver);
void st7789_fill(st7789_t *driver, const void* data);
void st7789_uninit(st7789_t *driver);
