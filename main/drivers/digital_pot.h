/**
 * @file digital_pot.h
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 * 
*/

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifndef DIGITAL_POT_INDEX
#define DIGITAL_POT_INDEX   0xFF
#endif

bool digital_pot_init(void);
bool digital_pot_set(uint8_t index);
void digital_pot_uninit(void);