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

#define COL_A_MASK  0x01
#define COL_B_MASK  0x02
#define COL_C_MASK  0x04
#define LEFT_MASK   0x08
#define RIGHT_MASK  0x10

static pin_t custom_row_pins[] = MATRIX_ROW_PINS;
static pin_t custom_col_pins[] = MATRIX_COL_PINS

void matrix_init_custom(void)
{
    #ifdef BUTTON_KEY
    gpio_set_input_pullup(BUTTON_KEY);
    #endif

    // initialize row pins
    for (int i = 0; i < MATRIX_ROWS; i++) {
        gpio_set_output_pushpull(custom_row_pins[i]);
        gpio_write_pin(custom_row_pins[i], 0);
    }

    // initialize col pins
    gpio_set_output_pushpull(LEFT_EN_PIN);
    gpio_write_pin(LEFT_EN_PIN, 1);

    gpio_set_output_pushpull(COL_A_PIN);
    gpio_write_pin(COL_A_PIN, 0);
    gpio_set_output_pushpull(COL_B_PIN);
    gpio_write_pin(COL_B_PIN, 0);
    gpio_set_output_pushpull(COL_C_PIN);
    gpio_write_pin(COL_C_PIN, 0);

    // initialize opamp and capacity discharge
    gpio_set_output_pushpull(OPA_EN_PIN);
    gpio_write_pin(OPA_EN_PIN, 1);
    gpio_set_output_pushpull(DISCHARGE_PIN);
    gpio_write_pin(DISCHARGE_PIN, 0);
}

extern ADC_HandleTypeDef hadc1;

static uint32_t adc_read(void)
{
    uint32_t data = 0;
    if (HAL_ADC_Start(&hadc1) == HAL_OK) {
        HAL_ADC_PollForConversion(&hadc1, 1);
        if ((HAL_ADC_GetState(&hadc1) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC)
        {
            data = HAL_ADC_GetValue(&hadc1);
        } else {
            custom_matrix_debug("key sense failed\n");
        }
        HAL_ADC_Stop(&hadc1);
    }

    return data;
}

static bool sense_key(pin_t row)
{
    bool key_down = false;
    gpio_write_pin(DISCHARGE_PIN, 1);
    wait_us(2);
    gpio_write_pin(DISCHARGE_PIN, 0);
    gpio_write_pin(row, 1);
    uint32_t data = adc_read();
    if (data > SENSE_TH) {
        key_down = true;
        custom_matrix_debug("key down: 0x%lx, data=%d\n", row, data);
    } else {
        custom_matrix_debug("read key: data=%d\n", data);
    }

    // clean up
    gpio_write_pin(row, 0);
    wait_us(20);

    return key_down;
}

static bool scan_matrix(matrix_row_t *raw)
{
    bool changed = false;

    for (int col = 0; col < MATRIX_COLS; col++) {
        gpio_write_pin(LEFT_EN_PIN, (custom_col_pins[col]&LEFT_MASK) ? 0 : 1);
        gpio_write_pin(COL_A_PIN, (custom_col_pins[col]&COL_A_MASK) ? 1 : 0);
        gpio_write_pin(COL_B_PIN, (custom_col_pins[col]&COL_B_MASK) ? 1 : 0);
        gpio_write_pin(COL_C_PIN, (custom_col_pins[col]&COL_C_MASK) ? 1 : 0);

        for (int row = 0; row < MATRIX_ROWS-1; row++) {
            matrix_row_t last_row_value    = raw[row];
            matrix_row_t current_row_value = last_row_value;

            if (sense_key(custom_row_pins[row])) {
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

static bool scan_button_key(matrix_row_t* row)
{
    bool changed = false;
    matrix_row_t last_row_value    = row[0];
    matrix_row_t current_row_value = last_row_value;
        
    if(gpio_read_pin(BUTTON_KEY) == 0) {
        current_row_value |= 1;
    } else {
        current_row_value &= ~1;
    }

    if (last_row_value != current_row_value) {
        row[0] = current_row_value;
        changed = true;
    }

    return changed;
}

bool matrix_scan_custom(matrix_row_t* raw)
{
    bool changed = false;
    gpio_write_pin(LEFT_EN_PIN, 0);

#if SCAN_ONE
    changed = scan_one(raw);
#else
    changed = scan_matrix(raw);
    gpio_write_pin(LEFT_EN_PIN, 1);

    changed |= scan_button_key(&raw[4]);

#endif

    if (changed) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            custom_matrix_debug("row:%d-%x\n", row, matrix_get_row(row));
            //amk_printf("row:%d-%x\n", row, matrix_get_row(row));
        }
    }
    return changed;
}
