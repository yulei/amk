/**
 * @file amk_state.h
 * @brief matrix state scanning
 * 
 * @copyright Copyright (c) astro
 * 
*/

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifndef STATE_ALTERNATE_KEYCODE_LAYER
#define STATE_ALTERNATE_KEYCODE_LAYER 6
#endif

void state_matrix_init(void);
bool state_matrix_update(uint32_t row, uint32_t col, uint32_t value);

uint8_t state_matrix_get_debounce(bool press);
void state_matrix_update_debounce(uint8_t down, uint8_t up);
