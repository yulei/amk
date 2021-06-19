/**
 * @file is31fl3236.h
 * @brief driver for is31fl3236
 */

#pragma once

#include "i2c_led.h"

i2c_led_t *is31fl3236_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num);
void is31fl3236_uninit(i2c_led_t *driver);

void is31fl3236_set_color(i2c_led_t *driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue);
void is31fl3236_set_color_all(i2c_led_t *driver, uint8_t red, uint8_t green, uint8_t blue);
void is31fl3236_update_buffers(i2c_led_t *driver);

#define OUT_1 0x00
#define OUT_2 0x01
#define OUT_3 0x02
#define OUT_4 0x03
#define OUT_5 0x04
#define OUT_6 0x05
#define OUT_7 0x06
#define OUT_8 0x07
#define OUT_9 0x08
#define OUT_10 0x09
#define OUT_11 0x0A
#define OUT_12 0x0B
#define OUT_13 0x0C
#define OUT_14 0x0D
#define OUT_15 0x0E
#define OUT_16 0x0F
#define OUT_17 0x10
#define OUT_18 0x11
#define OUT_19 0x12
#define OUT_20 0x13
#define OUT_21 0x14
#define OUT_22 0x15
#define OUT_23 0x16
#define OUT_24 0x17
#define OUT_25 0x18
#define OUT_26 0x19
#define OUT_27 0x1A
#define OUT_28 0x1B
#define OUT_29 0x1C
#define OUT_30 0x1D
#define OUT_31 0x1E
#define OUT_32 0x1F
#define OUT_33 0x20
#define OUT_34 0x21
#define OUT_35 0x22
#define OUT_36 0x23
