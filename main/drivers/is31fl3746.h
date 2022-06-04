/**
 * @file is31fl3746.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdbool.h>

#include "i2c_led.h"

bool is31fl3746_available(uint8_t addr);
i2c_led_t *is31fl3746_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num);
void is31fl3746_uninit(i2c_led_t *driver);

void is31fl3746_set_color(i2c_led_t *driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue);
void is31fl3746_set_color_all(i2c_led_t *driver, uint8_t red, uint8_t green, uint8_t blue);
void is31fl3746_update_buffers(i2c_led_t *driver);

#define C1_S1 0x01
#define C2_S1 0x02
#define C3_S1 0x03
#define C4_S1 0x04
#define C5_S1 0x05
#define C6_S1 0x06
#define C7_S1 0x07
#define C8_S1 0x08
#define C9_S1 0x09
#define C10_S1 0x0A
#define C11_S1 0x0B
#define C12_S1 0x0C
#define C13_S1 0x0D
#define C14_S1 0x0E
#define C15_S1 0x0F
#define C16_S1 0x10
#define C17_S1 0x11
#define C18_S1 0x12

#define C1_S2 0x13
#define C2_S2 0x14
#define C3_S2 0x15
#define C4_S2 0x16
#define C5_S2 0x17
#define C6_S2 0x18
#define C7_S2 0x19
#define C8_S2 0x1A
#define C9_S2 0x1B
#define C10_S2 0x1C
#define C11_S2 0x1D
#define C12_S2 0x1E
#define C13_S2 0x1F
#define C14_S2 0x20
#define C15_S2 0x21
#define C16_S2 0x22
#define C17_S2 0x23
#define C18_S2 0x24

#define C1_S3 0x25
#define C2_S3 0x26
#define C3_S3 0x27
#define C4_S3 0x28
#define C5_S3 0x29
#define C6_S3 0x2A
#define C7_S3 0x2B
#define C8_S3 0x2C
#define C9_S3 0x2D
#define C10_S3 0x2E
#define C11_S3 0x2F
#define C12_S3 0x30
#define C13_S3 0x31
#define C14_S3 0x32
#define C15_S3 0x33
#define C16_S3 0x34
#define C17_S3 0x35
#define C18_S3 0x36

#define C1_S4 0x37
#define C2_S4 0x38
#define C3_S4 0x39
#define C4_S4 0x3A
#define C5_S4 0x3B
#define C6_S4 0x3C
#define C7_S4 0x3D
#define C8_S4 0x3E
#define C9_S4 0x3F
#define C10_S4 0x40
#define C11_S4 0x41
#define C12_S4 0x42
#define C13_S4 0x43
#define C14_S4 0x44
#define C15_S4 0x45
#define C16_S4 0x46
#define C17_S4 0x47
#define C18_S4 0x48
