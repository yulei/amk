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

// key down debounce time
#ifndef DOWN_DEBOUNCE
#define DOWN_DEBOUNCE    5
#endif

// key up debounce time
#ifndef UP_DEBOUNCE
#define UP_DEBOUNCE    5
#endif

#ifndef STATE_DEBOUNCE_PRESS_LAYER
#define STATE_DEBOUNCE_PRESS_LAYER 4
#endif

#ifndef STATE_DEBOUNCE_RELEASE_LAYER
#define STATE_DEBOUNCE_RELEASE_LAYER 5
#endif

#ifndef STATE_ALTERNATE_KEYCODE_LAYER
#define STATE_ALTERNATE_KEYCODE_LAYER 6
#endif

void state_matrix_init(void);
bool state_matrix_update(uint32_t row, uint32_t col, uint32_t value);
