/**
 * matrix.c
 *  keyboard matrix scanning implementation
 */

#include <string.h>
#include "gpio_pin.h"
#include "matrix.h"
#include "print.h"
#include "timer.h"
#include "wait.h"

#ifndef DEBOUNCE
#   define DEBOUNCE 5
#endif

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];
static pin_t col_pins[] = MATRIX_COL_PINS;
static pin_t row_pins[] = MATRIX_ROW_PINS;

static bool debouncing = false;
static uint16_t debouncing_time = 0;

static void init_pins(void);


void matrix_init(void)
{
    // initialize row and col
    init_pins();

    // initialize matrix state: all keys off
    memset(&matrix[0], 0, sizeof(matrix));
    memset(&matrix_debouncing[0], 0, sizeof(matrix_debouncing));
}

uint8_t matrix_scan(void)
{
    bool changed = false;
    for (int col = 0; col < MATRIX_COLS; col++) {
        gpio_write_pin(col_pins[col], 1);
        wait_us(30);

        for(uint8_t row = 0; row< MATRIX_ROWS; row++) {
            matrix_row_t last_row_value    = matrix_debouncing[row];
            matrix_row_t current_row_value = last_row_value;

            if (gpio_read_pin(row_pins[row])) {
                current_row_value |= (1 << col);
            } else {
                current_row_value &= ~(1 << col);
            }

            if (last_row_value != current_row_value) {
                matrix_debouncing[row] = current_row_value;
                changed = true;
            }
        }
        gpio_write_pin(col_pins[col], 0);
    }

    if (changed && !debouncing) {
        debouncing = true;
        debouncing_time = timer_read();
    }

    if (debouncing && timer_elapsed(debouncing_time) > DEBOUNCE) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            matrix[row] = matrix_debouncing[row];
        }
        debouncing = false;
    }

    return 1;
}

bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 0123456789ABCDEF\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        pbin_reverse16(matrix_get_row(row));
        print("\n");
    }
}

static void  init_pins(void)
{
    // don't need pullup/down, since it's pulled down in hardware
    for (int col = 0; col < MATRIX_COLS; col++) {
        gpio_set_output_pushpull(col_pins[col]);
        gpio_write_pin(col_pins[col], 0);
    }
    for (int row = 0; row < MATRIX_ROWS; row++) {
        gpio_set_input_pulldown(row_pins[row]);
    }
}