/**
 * @file amk_apc.h
 * @brief actuation point control and rapid trigger
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
*/

#pragma once

#include <stdbool.h>
#include <stdint.h>

void apc_matrix_init(void);
uint32_t apc_matrix_calibrate(uint32_t row, uint32_t col, uint32_t value);
bool apc_matrix_update(uint32_t row, uint32_t col, uint32_t value, bool *valid);
void apc_matrix_update_interval(void);
void apc_matrix_update_interval_at(uint32_t row, uint32_t col);
void apc_matrix_update_sensitivity(void);
