/**
 * @file is31fl3193.h
 * @brief driver for is31fl3193
 */

#pragma once

#include <stdbool.h>

#include "i2c_led.h"

bool is31fl3193_available(uint8_t addr);
i2c_led_t *is31fl3193_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num);
void is31fl3193_uninit(i2c_led_t *driver);

void is31fl3193_set_color(i2c_led_t *driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue);
void is31fl3193_set_color_all(i2c_led_t *driver, uint8_t red, uint8_t green, uint8_t blue);
bool is31fl3193_update_buffers(i2c_led_t *driver);

#define OUT_1_93 0x00
#define OUT_2_93 0x01
#define OUT_3_93 0x02
