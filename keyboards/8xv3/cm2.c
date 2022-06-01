/**
 * @file custom_matrix.c
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

static pin_t custom_row_pins[] = {ROW_6_PIN, ROW_1_PIN, ROW_2_PIN, ROW_3_PIN, ROW_4_PIN, ROW_5_PIN};
static pin_t custom_col_pins[] = {LEFT_MASK|5, LEFT_MASK|7, LEFT_MASK|6, LEFT_MASK|4, LEFT_MASK|2, LEFT_MASK|1, LEFT_MASK|0, LEFT_MASK|3,
                                RIGHT_MASK|5, RIGHT_MASK|7, RIGHT_MASK|6, RIGHT_MASK|4, RIGHT_MASK|2, RIGHT_MASK|1, RIGHT_MASK|0, RIGHT_MASK|3};

void matrix_init_custom(void)
{
    // initialize row pins
    for (int i = 0; i < MATRIX_ROWS; i++) {
        //gpio_set_input_pulldown(custom_row_pins[i]);
        gpio_set_output_pushpull(custom_row_pins[i]);
        gpio_write_pin(custom_row_pins[i], 1);
    }

    // initialize col pins
    gpio_set_output_pushpull(LEFT_EN_PIN);
    gpio_write_pin(LEFT_EN_PIN, 1);
    gpio_set_output_pushpull(RIGHT_EN_PIN);
    gpio_write_pin(RIGHT_EN_PIN, 1);

    gpio_set_output_pushpull(COL_A_PIN);
    gpio_write_pin(COL_A_PIN, 0);
    gpio_set_output_pushpull(COL_B_PIN);
    gpio_write_pin(COL_B_PIN, 0);
    gpio_set_output_pushpull(COL_C_PIN);
    gpio_write_pin(COL_C_PIN, 0);

    //gpio_set_output_pushpull(KEY_PIN);
    //gpio_write_pin(KEY_PIN, 0);
    gpio_set_input_pullup(KEY_PIN);
    //gpio_write_pin(KEY_PIN, 0);
}

bool matrix_scan_custom(matrix_row_t* raw)
{
    //return false;

    bool changed = false;

    for (int col = 0; col < MATRIX_COLS; col++) {

        gpio_write_pin(COL_A_PIN, (custom_col_pins[col]&COL_A_MASK) ? 1 : 0);
        gpio_write_pin(COL_B_PIN, (custom_col_pins[col]&COL_B_MASK) ? 1 : 0);
        gpio_write_pin(COL_C_PIN, (custom_col_pins[col]&COL_C_MASK) ? 1 : 0);
        gpio_write_pin(LEFT_EN_PIN, (custom_col_pins[col]&LEFT_MASK) ? 0 : 1);
        gpio_write_pin(RIGHT_EN_PIN, (custom_col_pins[col]&RIGHT_MASK) ? 0 : 1);
        //gpio_write_pin(KEY_PIN, 1);
        //wait_us(20);
        for (int row = 0; row < MATRIX_ROWS; row++) {
            //if (row==2) continue;
            matrix_row_t last_row_value    = raw[row];
            matrix_row_t current_row_value = last_row_value;
            gpio_write_pin(custom_row_pins[row], 0);
            wait_us(10);

            //if (gpio_read_pin(custom_row_pins[row])) {
            if (gpio_read_pin(KEY_PIN)==0) {
                current_row_value |= (1 << col);
            } else {
                current_row_value &= ~(1 << col);
            }

            if (last_row_value != current_row_value) {
                raw[row] = current_row_value;
                changed = true;
            }
            gpio_write_pin(custom_row_pins[row], 1);
            //wait_us(10);
        }
        //gpio_write_pin(KEY_PIN, 0);
    }

    return changed;
}
