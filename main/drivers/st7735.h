
/**
 * @file st7735.h
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
} st7735_t;

void st7735_init(st7735_t *driver);
void st7735_fill_rect(st7735_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
void st7735_fill_rect_async(st7735_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
bool st7735_fill_ready(st7735_t *driver);
void st7735_release(st7735_t *driver);
void st7735_fill(st7735_t *driver, const void* data);
void st7735_uninit(st7735_t *driver);
