/**
 * ssd1357.h
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "gpio_pin.h"

typedef struct {
#ifdef SCREEN_POWER_PIN
    pin_t power;
#endif
    pin_t reset;
    pin_t cs;
    pin_t dc;
} ssd1357_t;

void ssd1357_init(ssd1357_t* driver);
void ssd1357_fill_rect(ssd1357_t* driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size);
void ssd1357_uninit(ssd1357_t* driver);

void ssd1357_fill(ssd1357_t* driver, const void* data);