/**
 * @file custom_matrix.c
 */

#include "matrix.h"
#include "wait.h"
#include "led.h"

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

static pin_t row_pins[] = MATRIX_ROW_PINS;
static pin_t col_pins[] = MATRIX_COL_PINS;
static port_t col_port = PORT_C;

void matrix_init_custom(void)
{
    for (int r = 0; r < MATRIX_ROWS; r++) {
        gpio_set_output_pushpull(row_pins[r]);
        gpio_write_pin(row_pins[r], 0);
    }

    for (int c = 0; c < MATRIX_COLS; c++) {
        gpio_set_input_pulldown(col_pins[c]);
    }
}

bool matrix_scan_custom(matrix_row_t* raw)
{
    bool changed = false;
    for (int row = 0; row < MATRIX_ROWS; row++) {
        gpio_write_pin(row_pins[row], 1);
        wait_us(10);

        matrix_row_t last_row_value    = raw[row];
        matrix_row_t current_row_value = ((matrix_row_t)gpio_read_port(col_port));
        if (last_row_value ^ current_row_value) {
            changed = true;
            raw[row] = current_row_value;
        }
        gpio_write_pin(row_pins[row], 0);
    }

    return changed;
}
