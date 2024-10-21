/**
 * @file hdc1080.h
 * @brief driver for hdc1080
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

bool hdc1080_init(void);
bool hdc1080_read(uint8_t* temperature, uint8_t* humidity);
void hdc1080_shutdown(void);