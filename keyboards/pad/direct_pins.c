/**
 * @file direct_pins.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 */

#include "quantum.h"

#include "amk_utils.h"
#include "amk_gpio.h"
#include "amk_printf.h"

#ifndef CUSTOM_MATRIX_DEBUG
#define CUSTOM_MATRIX_DEBUG 0
#endif

#if CUSTOM_MATRIX_DEBUG
#define custom_matrix_debug  amk_printf
#else
#define custom_matrix_debug(...)
#endif

static pin_t direct_pins[] = MATRIX_DIRECT_PINS;

void matrix_init_custom(void)
{
    for (int i = 0; i < AMK_ARRAY_SIZE(direct_pins); i++) {
        gpio_set_input_pullup(direct_pins[i]);
    }
}

bool matrix_scan_custom(matrix_row_t* raw)
{
    bool changed = false;

    for (int col = 0; col < MATRIX_COLS; col++) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            matrix_row_t last_row_value    = raw[row];
            matrix_row_t current_row_value = last_row_value;

            if (gpio_read_pin(direct_pins[row*MATRIX_COLS+col]) == 0) {
                current_row_value |= (1 << col);
            } else {
                current_row_value &= ~(1 << col);
            }

            if (last_row_value != current_row_value) {
                raw[row] = current_row_value;
                changed = true;
            }
        }
    }

    return changed;
}
