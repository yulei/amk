/**
 * @file aw9523b.h
 * @brief driver for aw9523b
 */

#pragma once

#include <stdbool.h>
#include "i2c_led.h"

#ifndef AW9523B_ADDR
#define AW9523B_ADDR    0xB6
#endif

bool aw9523b_available(uint8_t addr);

i2c_led_t *aw9523b_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num);
void aw9523b_uninit(i2c_led_t *driver);

void aw9523b_set_color(i2c_led_t *driver, int index, uint8_t red, uint8_t green, uint8_t blue);
void aw9523b_set_color_all(i2c_led_t *driver, uint8_t red, uint8_t green, uint8_t blue);
void aw9523b_update_buffers(i2c_led_t *driver);

#define AW_P10      0x0
#define AW_P11      0x1
#define AW_P12      0x2
#define AW_P13      0x3
#define AW_P00      0x4
#define AW_P01      0x5
#define AW_P02      0x6
#define AW_P03      0x7
#define AW_P04      0x8
#define AW_P05      0x9
#define AW_P06      0xA
#define AW_P07      0xB
#define AW_P14      0xC
#define AW_P15      0xD
#define AW_P16      0xE
#define AW_P17      0xF
