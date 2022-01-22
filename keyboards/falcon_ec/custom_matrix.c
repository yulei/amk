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

static pin_t custom_row_pins[] = {ROW_3_PIN, ROW_1_PIN, ROW_4_PIN, ROW_2_PIN};
static pin_t custom_col_pins[] = {COL_C_PIN, COL_B_PIN, COL_A_PIN};

void matrix_init_custom(void)
{
    #ifdef CAPS_LED_PIN
    gpio_set_output_pushpull(CAPS_LED_PIN);
    gpio_write_pin(CAPS_LED_PIN, 0);
    #endif

    #ifdef RGB_EN_PIN
    gpio_set_output_pushpull(RGB_EN_PIN);
    gpio_write_pin(RGB_EN_PIN, 1);
    #endif

    //power on switch board
    #ifdef POWER_PIN
    gpio_set_output_pushpull(POWER_PIN);
    gpio_write_pin(POWER_PIN, 1);

    wait_ms(100);
    #endif

    // initialize row pins
    gpio_set_output_pushpull(ROW_1_PIN);
    gpio_write_pin(ROW_1_PIN, 0);
    gpio_set_output_pushpull(ROW_2_PIN);
    gpio_write_pin(ROW_2_PIN, 0);
    gpio_set_output_pushpull(ROW_3_PIN);
    gpio_write_pin(ROW_3_PIN, 0);
    gpio_set_output_pushpull(ROW_4_PIN);
    gpio_write_pin(ROW_4_PIN, 0);

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

    // initialize opamp and capacitor discharge
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
    gpio_write_pin(DISCHARGE_PIN, 0);
    wait_us(1);
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
    gpio_write_pin(DISCHARGE_PIN, 1);
    wait_us(30);
    return key_down;
}

#define SCAN_ONE 0
#if SCAN_ONE
bool scan_one(matrix_row_t *raw)
{
    bool changed = false;
    uint8_t col = 0;

    gpio_write_pin(COL_C_PIN, 0);
    gpio_write_pin(COL_B_PIN, 1);
    gpio_write_pin(COL_A_PIN, 0);

    for (int row = 0; row < 1 /*MATRIX_ROWS*/; row++) {
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

    return changed;
}
#else
static bool scan_half(matrix_row_t *raw, bool right)
{
    bool changed = false;
    uint8_t col = right ? 8 : 0;
    //wait_us(10);

    for (int x = 0; x < 2; x++) {
        gpio_write_pin(custom_col_pins[0], x);
        for (int y = 0; y < 2; y++) {
            gpio_write_pin(custom_col_pins[1], y);
            for (int z = 0; z < 2; z++) {
                gpio_write_pin(custom_col_pins[2], z);
                //wait_us(10);

                for (int row = 0; row < MATRIX_ROWS; row++) {
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
                col++;
            }
        }
    }

    return changed;
}
#endif

bool matrix_scan_custom(matrix_row_t* raw)
{
    bool changed = false;
    // scan left
    gpio_write_pin(LEFT_EN_PIN, 0);
    gpio_write_pin(RIGHT_EN_PIN, 1);

#if SCAN_ONE
    changed = scan_one(raw);
#else
    changed = scan_half(raw, false);

    // scan right 
    gpio_write_pin(LEFT_EN_PIN, 1);
    gpio_write_pin(RIGHT_EN_PIN, 0);

    changed |= scan_half(raw, true);
    // turn off right 
    gpio_write_pin(RIGHT_EN_PIN, 1);

#endif

    //changed = false;
    if (changed) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            custom_matrix_debug("row:%d-%x\n", row, matrix_get_row(row));
        }
    }
    return changed;
}
