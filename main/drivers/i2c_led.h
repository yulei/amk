/**
 *  @file i2c_led.h
 *  @brief led driver with i2c interface
 */

#pragma once

#include <stdint.h>

typedef struct {
    uint8_t     index;
    uint8_t     addr;
    uint8_t     led_start;
    uint8_t     led_num;
    void        *data;       
} i2c_led_t;