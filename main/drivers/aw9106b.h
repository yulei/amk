/**
 * @file aw9106.h
 * @brief driver for aw9106, current support 2 drivers
 */

#pragma once

#include <stdint.h>
#include "rgb_color.h"

//#ifndef AW9106B_ADDR1
//    #define AW9106B_ADDR1   0xB6
//#endif

//#ifndef AW9106B_ADDR2
//    #define AW9106B_ADDR2   0xB2
//#endif

#define AW9106B_CTL         0x11
#define AW9106B_CPMD_A      0x12
#define AW9106B_CPMD_B      0x13
#define AW9106B_EN_BRE      0x14

#define AW9106B_PWM_BASE    0x20
#define AW9106B_DIM0        0x20
#define AW9106B_DIM1        0x21
#define AW9106B_DIM2        0x22
#define AW9106B_DIM3        0x23
#define AW9106B_DIM4        0x24
#define AW9106B_DIM5        0x25

#define AW9106B_RESET       0x7F
#define AW9106B_PWM_SIZE    6

extern rgb_led_t g_aw9106b_leds[];

void aw9106b_init(uint8_t addr, uint8_t index);

void aw9106b_set_color(int index, uint8_t red, uint8_t green, uint8_t blue);
void aw9106b_set_color_all(uint8_t red, uint8_t green, uint8_t blue);
void aw9106b_update_buffers(uint8_t addr, uint8_t index);

void aw9106b_uninit(uint8_t addr, uint8_t index);
