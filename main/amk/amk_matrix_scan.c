/**
 * @file amk_matrix_scan.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
*/

#include "matrix.h"
#include "wait.h"

#include "amk_gpio.h"
#include "amk_printf.h"

#ifdef STATE_SCAN_ENABLE
#include "amk_state.h"
#endif

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

#ifndef SCAN_PORT
#define SCAN_PORT           PORT_C
#endif

#ifndef SCAN_IO_DELAY_US
#define SCAN_IO_DELAY_US    5
#endif

void matrix_init_custom(void)
{
    for (int r = 0; r < MATRIX_ROWS; r++) {
        gpio_set_output_pushpull(row_pins[r]);
#ifdef SCAN_ROW2COL
        gpio_write_pin(row_pins[r], 0);
#else
        gpio_write_pin(row_pins[r], 1);
#endif
    }

    for (int c = 0; c < MATRIX_COLS; c++) {
#ifdef SCAN_ROW2COL
        gpio_set_input_pulldown(col_pins[c]);
#else
        gpio_set_input_pullup(col_pins[c]);
#endif
    }

#ifdef STATE_SCAN_ENABLE
        state_matrix_init();
#endif
}

static matrix_row_t read_current_row(void)
{
    matrix_row_t current = 0;

#ifdef PORT_SCAN_ENABLE
    #ifdef SCAN_ROW2COL
    current = ((matrix_row_t)gpio_read_port(SCAN_PORT));
    #else
    current = ~((matrix_row_t)gpio_read_port(SCAN_PORT));
    #endif
#else
    for (int col = 0; col < MATRIX_COLS; col++) {
#ifdef SCAN_ROW2COL
        int val = gpio_read_pin(col_pins[col]);
#else
        int val = !gpio_read_pin(col_pins[col]);
#endif
        if (val) {
            current |= 1 << col;
        }
    }
#endif

    return current;
}

bool matrix_scan_custom(matrix_row_t* raw)
{
    bool changed = false;
    for (int row = 0; row < MATRIX_ROWS; row++) {
#ifdef SCAN_ROW2COL
        gpio_write_pin(row_pins[row], 1);
#else
        gpio_write_pin(row_pins[row], 0);
#endif
        wait_us(SCAN_IO_DELAY_US);

        matrix_row_t last_row_value    = raw[row];
        matrix_row_t current_row_value = read_current_row();

#ifdef STATE_SCAN_ENABLE
        for (int col = 0; col < MATRIX_COLS; col++) {
            if (state_matrix_update(row, col, current_row_value & (1<<col))) {
                raw[row] |= (1<<col);
            } else {
                raw[row] &= ~(1<<col);
            }
        }
        if (last_row_value ^ raw[row]) {
            changed = true;
        }
#else
        if (last_row_value ^ current_row_value) {
            changed = true;
            raw[row] = current_row_value;
        }
#endif
#ifdef SCAN_ROW2COL
        gpio_write_pin(row_pins[row], 0);
#else
        gpio_write_pin(row_pins[row], 1);
#endif
    }

    return changed;
}
