
/**
 * @file rm67160.h
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "amk_gpio.h"

typedef struct {
#ifdef POWER_CHIP_PIN
    pin_t ctrl;
#endif
#ifdef SCREEN_POWER_PIN
    pin_t power;
#endif
    pin_t reset;
    pin_t cs;
    pin_t dc;
} rm67160_t;

void rm67160_init(rm67160_t *driver);
void rm67160_fill_rect(rm67160_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
void rm67160_fill_rect_async(rm67160_t *driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
bool rm67160_fill_ready(rm67160_t *driver);
void rm67160_release(rm67160_t *driver);
void rm67160_fill(rm67160_t *driver, const void* data);
void rm67160_uninit(rm67160_t *driver);
