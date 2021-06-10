/**
 * @file awinic.h
 * @brief commond definitions for awinic rgb led driver
 */

#pragma once

#include "rgb_common.h"

typedef struct {
    uint8_t index;
    uint8_t addr;
    uint8_t led_num;
    void *data;
} awinic_t;
