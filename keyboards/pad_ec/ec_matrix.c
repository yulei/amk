/**
 * @file ec_matrix.c
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
/*
#define L_MASK      0x08
#define R_MASK      0x10
*/

static pin_t custom_row_pins[] = MATRIX_ROW_PINS;
static pin_t custom_col_pins[] = MATRIX_COL_PINS;
#ifdef CDC_ENABLE
#include "keyboard.h"
struct sense_value {
    int min;
    int max;
};

static struct sense_value matrix_values[MATRIX_ROWS][MATRIX_COLS] = 
{
    {{5120,0},{5120,0},{5120,0},{5120,0},{5120,0}},
    {{5120,0},{5120,0},{5120,0},{5120,0},{5120,0}},
    {{5120,0},{5120,0},{5120,0},{5120,0},{5120,0}},
    {{5120,0},{5120,0},{5120,0},{5120,0},{5120,0}},
};

extern void cdc_output(const char* buf, uint32_t size);

static void dump_matrix_row(int row)
{
    static char buf[128];

    //for (int i = 0; i < MATRIX_ROWS; i++) {
        for (int j = 0; j < MATRIX_COLS; j++) {
            int count = sprintf(buf, "[%d-%d]: min=%d, max=%d ", row, j, matrix_values[row][j].min, matrix_values[row][j].max);
            cdc_output(buf, count);
        }
        cdc_output("\n", 1);
    //}
}

void hook_matrix_change_kb(keyevent_t event)
{
    if (!event.pressed) {
        dump_matrix_row(event.key.row);
    }
}

#endif

void matrix_init_custom(void)
{
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
    for (int i = 0; i < MATRIX_ROWS; i++) {
        gpio_set_output_pushpull(custom_row_pins[i]);
        gpio_write_pin(custom_row_pins[i], 0);
    }

    // initialize col pins
#ifdef LEFT_EN_PIN
    gpio_set_output_pushpull(LEFT_EN_PIN);
    gpio_write_pin(LEFT_EN_PIN, 1);
#endif
#ifdef RIGHT_EN_PIN
    gpio_set_output_pushpull(RIGHT_EN_PIN);
    gpio_write_pin(RIGHT_EN_PIN, 1);
#endif

    gpio_set_output_pushpull(COL_A_PIN);
    gpio_write_pin(COL_A_PIN, 0);
    gpio_set_output_pushpull(COL_B_PIN);
    gpio_write_pin(COL_B_PIN, 0);
    gpio_set_output_pushpull(COL_C_PIN);
    gpio_write_pin(COL_C_PIN, 0);

    // initialize opamp and capacity discharge
#ifdef OPA_EN_PIN
    gpio_set_output_pushpull(OPA_EN_PIN);
    gpio_write_pin(OPA_EN_PIN, 1);
#endif
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

static bool sense_key(pin_t row, bool on, uint32_t *value)
//static bool sense_key(pin_t row, bool on)
{
    bool key_down = false;
    //gpio_write_pin(DISCHARGE_PIN, 0);
    //wait_us(10);
    //gpio_write_pin(DISCHARGE_PIN, 0);
    gpio_write_pin(DISCHARGE_PIN, 0);
    wait_us(DISCHARGE_WAIT_PRE);
    gpio_set_input_floating(DISCHARGE_PIN);
    gpio_write_pin(row, 1);
    uint32_t data = adc_read();
    // press to release
    if (on) {
        if (data > EC_TH_LOW) {
            key_down = true;
        }
    } else {
        if (data > EC_TH_HIGH) {
            key_down = true;
        }
    }
    if (key_down) {
        custom_matrix_debug("key down: 0x%lx, data=%d\n", row, data);
        //amk_printf("key down: 0x%lx, data=%d\n", row, data);
    }

    // clean up
    gpio_set_output_pushpull(DISCHARGE_PIN);
   // gpio_write_pin(DISCHARGE_PIN, 0);
    gpio_write_pin(row, 0);
    wait_us(DISCHARGE_WAIT_POST);
    *value = data;

    return key_down;
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
#if SCAN_ONE
    changed = scan_one(raw);
#else
    for (int col = 0; col < MATRIX_COLS; col++) {

        gpio_write_pin(COL_A_PIN, (custom_col_pins[col]&COL_A_MASK) ? 1 : 0);
        gpio_write_pin(COL_B_PIN, (custom_col_pins[col]&COL_B_MASK) ? 1 : 0);
        gpio_write_pin(COL_C_PIN, (custom_col_pins[col]&COL_C_MASK) ? 1 : 0);

        if (custom_col_pins[col]&LEFT_MASK) {
            gpio_write_pin(LEFT_EN_PIN,  0);
        }
        #ifdef RIGHT_EN_PIN
        if (custom_col_pins[col]&R_MASK) {
            gpio_write_pin(RIGHT_EN_PIN, 0);
        }
        #endif

        for (int row = 0; row < MATRIX_ROWS-1; row++) {
            matrix_row_t last_row_value    = raw[row];
            matrix_row_t current_row_value = last_row_value;
            uint32_t value = 0;

            if (sense_key(custom_row_pins[row], (last_row_value&col), &value)) {
                current_row_value |= (1 << col);
            } else {
                current_row_value &= ~(1 << col);
            }

            if (last_row_value != current_row_value) {
                raw[row] = current_row_value;
                changed = true;
            }
            #ifdef CDC_ENABLE
            if (matrix_values[row][col].min > value) {
                matrix_values[row][col].min = value;
            }
            if (matrix_values[row][col].max < value) {
                matrix_values[row][col].max = value;
            }
            #endif
        }
        gpio_write_pin(LEFT_EN_PIN,  1);
        #ifdef RIGHT_EN_PIN
        gpio_write_pin(RIGHT_EN_PIN, 1);
        #endif
    }
#endif

    changed |= scan_button_key(&raw[4]);

    if (changed) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            custom_matrix_debug("row:%d-%x\n", row, matrix_get_row(row));
        }
    }
    return changed;
}