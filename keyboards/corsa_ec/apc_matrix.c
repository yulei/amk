/**
 * @file apc_matrix.c
 */

#include "quantum.h"

#include "amk_gpio.h"
#include "amk_utils.h"
#include "amk_printf.h"

#ifndef CUSTOM_MATRIX_DEBUG
#define CUSTOM_MATRIX_DEBUG 1
#endif

#if CUSTOM_MATRIX_DEBUG
#define custom_matrix_debug  amk_printf
#else
#define custom_matrix_debug(...)
#endif


enum apc_key_state
{
    APC_KEY_OFF,
    APC_KEY_PRESSING,
    APC_KEY_ON,
    APC_KEY_RELEASING,
};

struct apc_key
{
    uint32_t state;
    uint32_t min;
    uint32_t max;
    uint32_t cur;
    uint32_t trigger;
};

#define APC_KEY_STATE_DEFAULT   APC_KEY_OFF
#define APC_KEY_MIN_DEFAULT     4096
#define APC_KEY_MAX_DEFAULT     0
#define APC_KEY_CUR_DEFAULT     0
#define APC_KEY_TRIGGER_DEFAULT 0

#define APC_INTERVAL_MIN        400
#define APC_INTERVAL_MAX        2600

#define APC_INTERVAL_DEFAULT    1024
#define APC_THRESHOLD           200
#define APC_KEYMAP_INDEX        3

static struct apc_key apc_matrix[MATRIX_ROWS][MATRIX_COLS];

#include "dynamic_keymap.h"

uint32_t apc_get_key_interval(uint32_t row, uint32_t col)
{
    uint16_t keycode = dynamic_keymap_get_keycode(APC_KEYMAP_INDEX, row, col);
    if (keycode < KC_F1 || keycode > KC_F10) {
        keycode = KC_F7;
    }

    uint32_t interval = APC_INTERVAL_MIN + ((APC_INTERVAL_MAX-APC_INTERVAL_MIN)/(KC_F10-KC_F1))*(keycode-KC_F1);
    //custom_matrix_debug("APC INTERVAL: value=%d, row=%d, col=%d\n", interval, row, col);
    return interval;
}

static void apc_matrix_init(void)
{
    for (int i = 0; i < MATRIX_ROWS; i++) {
        for (int j = 0; j < MATRIX_COLS; j++) {
            apc_matrix[i][j].state = APC_KEY_STATE_DEFAULT;
            apc_matrix[i][j].min = APC_KEY_MIN_DEFAULT;
            apc_matrix[i][j].max = APC_KEY_MAX_DEFAULT;
            apc_matrix[i][j].cur = APC_KEY_CUR_DEFAULT;
            apc_matrix[i][j].trigger = APC_KEY_TRIGGER_DEFAULT;
        }
    }
}

bool is_adc_value_valid(uint32_t value)
{
    if (value < 150 || value > 2560) return false;
    return true;
}

static bool apc_matrix_update(uint32_t row, uint32_t col, uint32_t value)
{
    struct apc_key* apc = &apc_matrix[row][col];
    if (is_adc_value_valid(value)) {
        if (apc->min > value) apc->min = value;
        if (apc->max < value) apc->max = value;
        if (apc->cur < apc->min) apc->cur = apc->min;

        switch(apc->state) {
        case APC_KEY_OFF:
            if (value > apc->cur + APC_THRESHOLD)  {
                apc->state = APC_KEY_PRESSING;
                custom_matrix_debug("APC from OFF to PRESSING, value=%d, min=%d, max=%d, trigger=%d\n", value, apc->min, apc->max, apc->trigger);
            }
            break;
        case APC_KEY_ON:
            if (value + APC_THRESHOLD < apc->trigger)  {
                apc->state = APC_KEY_RELEASING;
                custom_matrix_debug("APC from ON to RELESASING, value=%d, min=%d, max=%d, trigger=%d\n", value, apc->min, apc->max, apc->trigger);
            }
            break;
        case APC_KEY_PRESSING: 
            {
                uint32_t interval = apc_get_key_interval(row, col);
                if (value > apc->min + interval) {
                    apc->trigger = value;
                    apc->state = APC_KEY_ON;
                    custom_matrix_debug("APC from PRESSING to ON, value=%d, min=%d, max=%d, trigger=%d\n", value, apc->min, apc->max, apc->trigger);
                    custom_matrix_debug("APC INTERVAL: value=%d\n", interval);
                }
            }
            break;
        case APC_KEY_RELEASING:
            if (value < apc->trigger) {
                apc->state = APC_KEY_OFF;
                custom_matrix_debug("APC from RELEASING to OFF, value=%d, min=%d, max=%d, trigger=%d\n", value, apc->min, apc->max, apc->trigger);
            }
            break;
        default:
            // never here
            break;
        }
        apc->cur = value;
    }

    return apc->state == APC_KEY_ON;
}

struct ec_mode_t{
    uint32_t low;
    uint32_t high;
};

#define EC_MODE_MAX 4
struct ec_mode_t ec_modes[EC_MODE_MAX] = {
    {768, 864},
    {1024, 1280},
    {864, 1024},
    {1280, 1444},
};

static uint32_t ec_mode_current = 0;

void ec_mode_iter(void)
{
    ec_mode_current = (ec_mode_current+1) % EC_MODE_MAX;
    custom_matrix_debug("ec mode changed to:%d\n", ec_mode_current);
    eeconfig_update_kb(ec_mode_current);
}

#define COL_A_MASK  0x01
#define COL_B_MASK  0x02
#define COL_C_MASK  0x04

static pin_t custom_row_pins[] = MATRIX_ROW_PINS;
static pin_t custom_col_pins[] = MATRIX_COL_PINS;

void matrix_init_custom(void)
{
    ec_mode_current = eeconfig_read_kb();
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
    for (int i = 0; i < AMK_ARRAY_SIZE(custom_row_pins); i++) {
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
    gpio_write_pin(COL_A_PIN, 1);
    gpio_set_output_pushpull(COL_B_PIN);
    gpio_write_pin(COL_B_PIN, 0);
    gpio_set_output_pushpull(COL_C_PIN);
    gpio_write_pin(COL_C_PIN, 1);

    // initialize opamp and capacity discharge
#ifdef OPA_EN_PIN
    gpio_set_output_pushpull(OPA_EN_PIN);
    gpio_write_pin(OPA_EN_PIN, 1);
#endif
    gpio_set_output_pushpull(DISCHARGE_PIN);
    gpio_write_pin(DISCHARGE_PIN, 0);

    apc_matrix_init();
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

static bool sense_key(pin_t strobe, bool on, uint32_t row, uint32_t col)
{
    bool key_down = false;
    gpio_set_input_floating(DISCHARGE_PIN);
    //wait_us(2);
    gpio_write_pin(strobe, 1);
    //wait_us(2);
    uint32_t data = adc_read();
    // press to release
#if 0
    if (on) {
        if (data > ec_modes[ec_mode_current].low) {
            key_down = true;
        }
    } else {
        if (data > ec_modes[ec_mode_current].high) {
            key_down = true;
        }
    }
#else
    key_down = apc_matrix_update(row, col, data);
#endif
    if (key_down) {
        //custom_matrix_debug("key down: 0x%lx, data=%d\n", row, data);
        //amk_printf("key down: 0x%lx, data=%d\n", row, data);
    }

    // clean up
    gpio_set_output_pushpull(DISCHARGE_PIN);
    gpio_write_pin(DISCHARGE_PIN, 0);
    gpio_write_pin(strobe, 0);
    wait_us(DISCHARGE_WAIT_POST);

    //return false;
    return key_down;
}

bool matrix_scan_custom(matrix_row_t* raw)
{
    bool changed = false;
#if SCAN_ONE
    changed = scan_one(raw);
#else
    gpio_write_pin(DISCHARGE_PIN, 0);
    wait_us(300);
    for (int col = 0; col < MATRIX_COLS; col++) {

        gpio_write_pin(COL_A_PIN, (custom_col_pins[col]&COL_A_MASK) ? 1 : 0);
        gpio_write_pin(COL_B_PIN, (custom_col_pins[col]&COL_B_MASK) ? 1 : 0);
        gpio_write_pin(COL_C_PIN, (custom_col_pins[col]&COL_C_MASK) ? 1 : 0);

        if (custom_col_pins[col]&L_MASK) {
            gpio_write_pin(LEFT_EN_PIN,  0);
        }
        if (custom_col_pins[col]&R_MASK) {
            gpio_write_pin(RIGHT_EN_PIN, 0);
        }

        for (int row = 0; row < MATRIX_ROWS; row++) {
            matrix_row_t last_row_value    = raw[row];
            matrix_row_t current_row_value = last_row_value;

            if (sense_key(custom_row_pins[row], (last_row_value&col), row, col)) {
                current_row_value |= (1 << col);
            } else {
                current_row_value &= ~(1 << col);
            }

            if (last_row_value != current_row_value) {
                raw[row] = current_row_value;
                changed = true;
            }
        }
        gpio_write_pin(LEFT_EN_PIN,  1);
        gpio_write_pin(RIGHT_EN_PIN, 1);
    }
#endif

/*
    gpio_set_output_pushpull(COL_A_PIN);
    gpio_write_pin(COL_A_PIN, 1);
    gpio_set_output_pushpull(COL_B_PIN);
    gpio_write_pin(COL_B_PIN, 0);
    gpio_set_output_pushpull(COL_C_PIN);
    gpio_write_pin(COL_C_PIN, 1);
    */

    if (changed) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            //custom_matrix_debug("row:%d-%x\n", row, matrix_get_row(row));
        }
    }
    return changed;
}
