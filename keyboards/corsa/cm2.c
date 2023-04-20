/**
 * @file cm2.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "matrix.h"
#include "wait.h"

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

#define COL_A_MASK  0x01
#define COL_B_MASK  0x02
#define COL_C_MASK  0x04
#define LEFT_MASK   0x08
#define RIGHT_MASK  0x10

static pin_t custom_row_pins[] = MATRIX_ROW_PINS;
static pin_t custom_col_pins[] = MATRIX_COL_PINS;

void matrix_init_custom(void)
{
    // initialize row pins
    for (int i = 0; i < MATRIX_ROWS; i++) {
        gpio_set_input_pullup(custom_row_pins[i]);
    }

    // initialize col pins
    gpio_set_output_pushpull(LEFT_EN_PIN);
    gpio_write_pin(LEFT_EN_PIN, 0);
    gpio_set_output_pushpull(RIGHT_EN_PIN);
    gpio_write_pin(RIGHT_EN_PIN, 0);

    gpio_set_output_pushpull(COL_A_PIN);
    gpio_write_pin(COL_A_PIN, 0);
    gpio_set_output_pushpull(COL_B_PIN);
    gpio_write_pin(COL_B_PIN, 0);
    gpio_set_output_pushpull(COL_C_PIN);
    gpio_write_pin(COL_C_PIN, 0);
}

bool matrix_scan_custom(matrix_row_t* raw)
{
    bool changed = false;

    for (int col = 0; col < MATRIX_COLS; col++) {

        gpio_write_pin(COL_A_PIN, (custom_col_pins[col]&COL_A_MASK) ? 1 : 0);
        gpio_write_pin(COL_B_PIN, (custom_col_pins[col]&COL_B_MASK) ? 1 : 0);
        gpio_write_pin(COL_C_PIN, (custom_col_pins[col]&COL_C_MASK) ? 1 : 0);
        gpio_write_pin(LEFT_EN_PIN, (custom_col_pins[col]&LEFT_MASK) ? 1 : 0);
        gpio_write_pin(RIGHT_EN_PIN, (custom_col_pins[col]&RIGHT_MASK) ? 1 : 0);
        wait_us(10);

        for (int row = 0; row < MATRIX_ROWS; row++) {
            matrix_row_t last_row_value    = raw[row];
            matrix_row_t current_row_value = last_row_value;

            if (!gpio_read_pin(custom_row_pins[row])) {
                current_row_value |= (1 << col);
            } else {
                current_row_value &= ~(1 << col);
            }

            if (last_row_value != current_row_value) {
                raw[row] = current_row_value;
                changed = true;
            }
        }

        gpio_write_pin(LEFT_EN_PIN, 0);
        gpio_write_pin(RIGHT_EN_PIN,  0);
    }

    return changed;
}
