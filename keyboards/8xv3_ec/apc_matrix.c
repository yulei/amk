/**
 * @file apc_matrix.c
 */

#include "matrix.h"
#include "wait.h"
#include "led.h"

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

#define COL_A_MASK  0x01
#define COL_B_MASK  0x02
#define COL_C_MASK  0x04
#define COL_D_MASK  0x08

extern ADC_HandleTypeDef    hadc1;

static pin_t custom_row_pins[] = MATRIX_ROW_PINS;
static pin_t custom_col_pins[] = MATRIX_COL_PINS;


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
    uint32_t last;
    uint32_t trigger;
};

#ifndef APC_KEY_MIN
#define APC_KEY_MIN             50
#endif

#ifndef APC_KEY_MAX
#define APC_KEY_MAX             3000
#endif

#define APC_KEY_STATE_DEFAULT   APC_KEY_OFF
#define APC_KEY_MIN_DEFAULT     APC_KEY_MAX
#define APC_KEY_MAX_DEFAULT     APC_KEY_MIN
#define APC_KEY_LAST_DEFAULT    0
#define APC_KEY_CUR_DEFAULT     0
#define APC_KEY_TRIGGER_DEFAULT 0

#define APC_INTERVAL_MIN        50
#define APC_INTERVAL_MAX        2048

#define APC_INTERVAL_DEFAULT    500
#define APC_THRESHOLD           100
#define APC_KEYMAP_INDEX        3

static struct apc_key apc_matrix[MATRIX_ROWS][MATRIX_COLS];

#ifdef DYNAMIC_KEYMAP_ENABLE
#include "dynamic_keymap.h"
#else
extern const uint16_t keymaps[][][];
#endif

uint32_t apc_get_key_interval(uint32_t row, uint32_t col)
{

#ifdef DYNAMIC_KEYMAP_ENABLE
    uint16_t keycode = dynamic_keymap_get_keycode(APC_KEYMAP_INDEX, row, col);
#else
    uint16_t keycode = keymap[APC_KEYMAP_INDEX][row][col];
#endif
    uint16_t index = 0xFFFF;
    if (keycode>=KC_F1 && keycode <= KC_F12) {
        index = keycode - KC_F1;
    } /*else if (keycode>=KC_F13 && keycode <= KC_F24) {
        index = 12 + keycode - KC_F13;
    }*/ else {
        index = 4;
    }

    uint32_t interval = APC_INTERVAL_MIN + ((APC_INTERVAL_MAX-APC_INTERVAL_MIN)/12)*(index);
    //uint32_t interval = APC_INTERVAL_MIN + ((apc_matrix[row][col].max-apc_matrix[row][col].min)/24)*(index);

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
            apc_matrix[i][j].last = APC_KEY_LAST_DEFAULT;
            apc_matrix[i][j].trigger = APC_KEY_TRIGGER_DEFAULT;
        }
    }
}

bool is_adc_value_valid(uint32_t value)
{
    if (value < APC_KEY_MIN || value > APC_KEY_MAX) return false;
    return true;
}

static bool apc_matrix_update(uint32_t row, uint32_t col, uint32_t value)
{
    struct apc_key* apc = &apc_matrix[row][col];
    if (is_adc_value_valid(value)) {
        if (apc->min > value) apc->min = value;
        if (apc->max < value) apc->max = value;
        if (apc->last == APC_KEY_LAST_DEFAULT) apc->last = value;
        apc->cur = value;

        switch(apc->state) {
        case APC_KEY_OFF:
            if (value > apc->min + APC_THRESHOLD)  {
                apc->state = APC_KEY_PRESSING;
                apc->last = value;
                //custom_matrix_debug("APC from OFF to PRESSING, value=%d, min=%d, max=%d, last=%d\n", value, apc->min, apc->max, apc->last);
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
                if (value > apc->last + interval) {
                    apc->trigger = value;
                    apc->state = APC_KEY_ON;
                    custom_matrix_debug("APC from PRESSING to ON, value=%d, min=%d, max=%d, trigger=%d\n", value, apc->min, apc->max, apc->trigger);
                    custom_matrix_debug("APC INTERVAL: value=%d\n", interval);
                } else {
                    if (value < apc->last) {
                        apc->state = APC_KEY_OFF;
                        apc->last = value;
                        //custom_matrix_debug("APC from PRESSING to OFF, value=%d, min=%d, max=%d, last=%d\n", value, apc->min, apc->max, apc->last);
                    } else {
                        //
                    }
                }
            }
            break;
        case APC_KEY_RELEASING:
            {
                uint32_t interval = apc_get_key_interval(row, col);
                if (value < apc->trigger+interval) {
                    apc->state = APC_KEY_OFF;
                    custom_matrix_debug("APC from RELEASING to OFF, value=%d, min=%d, max=%d, trigger=%d\n", value, apc->min, apc->max, apc->trigger);
                }
            }
            break;
        default:
            // never here
            break;
        }
    }

    return apc->state == APC_KEY_ON || apc->state == APC_KEY_RELEASING;
}

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

    apc_matrix_init();
}

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

static bool sense_key(uint32_t row, uint32_t col)
{
    bool key_down = false;
    //gpio_write_pin(DISCHARGE_PIN, 0);
    //wait_us(DISCHARGE_WAIT_PRE);

    gpio_set_input_floating(DISCHARGE_PIN);
    gpio_write_pin(custom_row_pins[row], 1);
    uint32_t adc_value = adc_read();

    key_down = apc_matrix_update(row, col, adc_value);
    if (key_down) {
        //custom_matrix_debug("key down: 0x%lx, data=%d\n", row, data);
        //amk_printf("key down: 0x%lx, data=%d\n", row, data);
    }

    // clean up
    gpio_set_output_pushpull(DISCHARGE_PIN);
    gpio_write_pin(DISCHARGE_PIN, 0);
    gpio_write_pin(custom_row_pins[row], 0);
    wait_us(DISCHARGE_WAIT_POST);

    return key_down;
}

bool matrix_scan_custom(matrix_row_t* raw)
{
    bool changed = false;
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

            if (sense_key(row, col)) {
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

    if (changed) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            //custom_matrix_debug("row:%d-%x\n", row, matrix_get_row(row));
        }
    }
    return changed;
}
