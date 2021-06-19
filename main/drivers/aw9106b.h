/**
 * @file aw9106.h
 * @brief driver for aw9106, current support 2 drivers
 */

#pragma once

#include "i2c_led.h"

i2c_led_t *aw9106b_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num);
void aw9106b_uninit(i2c_led_t *driver);

void aw9106b_set_color(i2c_led_t *driver, int index, uint8_t red, uint8_t green, uint8_t blue);
void aw9106b_set_color_all(i2c_led_t *driver, uint8_t red, uint8_t green, uint8_t blue);
void aw9106b_update_buffers(i2c_led_t *driver);

#define AW_DIM0         0x0
#define AW_DIM1         0x1
#define AW_DIM2         0x2
#define AW_DIM3         0x3
#define AW_DIM4         0x4
#define AW_DIM5         0x5
