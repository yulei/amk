/**
 * @file amk_dks.h
 * @brief dynamic keystroke
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>

void dks_matrix_init(void);
bool dks_matrix_parse_key(uint32_t row, uint32_t col, uint16_t key);
bool dks_matrix_valid(uint32_t row, uint32_t col);
bool dks_matrix_update(uint32_t row, uint32_t col, uint32_t value);
