/**
 * @file custom_matrix.c
 */

#include "matrix.h"
#include "wait.h"
#include "action.h"
#include "action_layer.h"

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
#define L_MASK      0x08
#define R_MASK      0x10

static pin_t custom_row_pins[] = {ROW_6_PIN, ROW_1_PIN, ROW_2_PIN, ROW_3_PIN, ROW_4_PIN, ROW_5_PIN};
static pin_t custom_col_pins[] = {L_MASK|3, L_MASK|0, L_MASK|1, L_MASK|2, L_MASK|4, L_MASK|6, L_MASK|7, L_MASK|5,
                                  R_MASK|3, R_MASK|0, R_MASK|1, R_MASK|2, R_MASK|4, R_MASK|6, R_MASK|7, R_MASK|5};

#define INVALID_MAX 0xFFFF
#define AUTO_MIN    0xFFFF
#define AUTO_MAX    0x0
#define EC_MAX      2560

#define STROKE_MIN  8
#define STROKE_MAX  20

#define STROKE_DEF  14

enum {
    EC_DEFAULT_MODE,
    EC_AUTO_MODE,
    EC_CALIBRATE_MODE,
};

typedef struct {
    uint16_t        min_def;        // calibrated min value
    uint16_t        max_def;        // calibrated max value 
    uint16_t        min_auto;       // auto update min value
    uint16_t        max_auto;       // auto update max value
    uint16_t        real;           // current value
    uint16_t        stroke;         // current stoke
} ec_key_t;

typedef struct {
    ec_key_t    keys[MATRIX_ROWS][MATRIX_COLS];
    uint8_t     mode;
} ec_matrix_t;

static ec_matrix_t ec_matrix = {
    {
        { 
            {87,2283,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {89,2173,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {81,2189,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {89,2440,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,2361,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,1745,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {85,1559,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {85,1637,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {87,1803,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,2122,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,2285,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,1797,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {90,1969,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,2367,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {89,2329,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,1802,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
        },
        {
            {87,1617,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,2191,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {90,2259,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {88,2493,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,2247,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {89,1869,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,2238,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {89,2448,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {81,2519,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {90,2345,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {88,2443,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {87,2528,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,INVALID_MAX/*180*/,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {87,2042,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {89,2101,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {88,2097,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
        },
        {
            {85,1933,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {92,2560,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {88,2336,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {87,2034,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,2278,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {87,2303,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,2149,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {88,2473,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {84,2295,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,2283,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {88,2252,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {88,2495,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {88,1418,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {85,INVALID_MAX/*145*/,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,2191,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,2174,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
        },
        {
            {86,2111,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {89,2443,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {87,2435,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {89,2367,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,2162,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {91,2478,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {85,2513,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {87,2398,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {88,2102,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {87,2247,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {89,2335,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {85,2371,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {89,2462,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {82,2307,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {87,2205,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {87,2196,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
        },
        {
            {82,INVALID_MAX/*234*/,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {101,1317,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {88,2256,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {88,2485,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {87,2538,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {88,2351,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {87,2390,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,2307,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {87,2394,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,2224,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,1829,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {85,INVALID_MAX/*303*/,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {112,2026,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {89,2200,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {88,2303,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {89,2232,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
        },
        {
            {86,1982,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,INVALID_MAX/*800*/,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {85,INVALID_MAX/*161*/,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {88,2267,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {82,2194,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {84,INVALID_MAX/*105*/,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {91,1517,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {83,INVALID_MAX/*188*/,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {86,INVALID_MAX/*280*/,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {183,2254,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {85,INVALID_MAX/*94*/,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {88,2328,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {87,2268,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {87,2316,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {92,2109,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
            {85,INVALID_MAX/*93*/,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},
        },
    },
    //EC_AUTO_MODE,
    //EC_CALIBRATE_MODE,
    EC_DEFAULT_MODE,
};

static void ec_matrix_init(ec_matrix_t *matrix);
static bool ec_matrix_update(ec_matrix_t *matrix, uint8_t row, uint8_t col, uint16_t value);
static void ec_matrix_dump_row(ec_matrix_t *matrix, uint8_t row);

void matrix_init_custom(void)
{
    ec_matrix_init(&ec_matrix);

    #ifdef DIRECT_KEY_PIN
    gpio_set_input_pullup(DIRECT_KEY_PIN);
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
    gpio_set_output_pushpull(ROW_6_PIN);
    gpio_write_pin(ROW_6_PIN, 0);
    gpio_set_output_pushpull(ROW_1_PIN);
    gpio_write_pin(ROW_1_PIN, 0);
    gpio_set_output_pushpull(ROW_2_PIN);
    gpio_write_pin(ROW_2_PIN, 0);
    gpio_set_output_pushpull(ROW_3_PIN);
    gpio_write_pin(ROW_3_PIN, 0);
    gpio_set_output_pushpull(ROW_4_PIN);
    gpio_write_pin(ROW_4_PIN, 0);
    gpio_set_output_pushpull(ROW_5_PIN);
    gpio_write_pin(ROW_5_PIN, 0);

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
    gpio_write_pin(DISCHARGE_PIN, 1);
}

extern ADC_HandleTypeDef hadc1;

static uint32_t adc_read(void)
{
    uint32_t data = 0;

    __disable_irq();
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
    __enable_irq();

#if 0
    if (HAL_ADC_Start(&hadc1) == HAL_OK) {
        HAL_ADC_PollForConversion(&hadc1, 1);
        if ((HAL_ADC_GetState(&hadc1) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC)
        {
            data += HAL_ADC_GetValue(&hadc1);
        } else {
            custom_matrix_debug("key sense failed\n");
        }
        HAL_ADC_Stop(&hadc1);
    }
#endif

    return data;
}

static bool sense_key(pin_t row, bool on, uint8_t row_index, uint8_t col_index)
{
    bool key_down = false;
    gpio_write_pin(DISCHARGE_PIN, 1);
    wait_us(25);
    gpio_write_pin(DISCHARGE_PIN, 0);
    wait_us(2);
    gpio_write_pin(row, 1);
    //wait_us(1);
    
    uint32_t data = adc_read();

    key_down = ec_matrix_update(&ec_matrix, row_index, col_index, data);

    gpio_write_pin(row, 0);
    gpio_write_pin(DISCHARGE_PIN, 1);
    wait_us(25);
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
#endif

#ifdef DIRECT_KEY_PIN
bool check_direct_pin(matrix_row_t* raw)
{
    matrix_row_t last_row_value    = raw[DIRECT_KEY_ROW];
    matrix_row_t current_row_value = last_row_value;
    if (!gpio_read_pin(DIRECT_KEY_PIN)) {
        current_row_value |= (1 << DIRECT_KEY_COL);
    } else {
        current_row_value &= ~(1 << DIRECT_KEY_COL);
    }
    if (last_row_value != current_row_value) {
        raw[DIRECT_KEY_ROW] = current_row_value;
        return true;
    }

    return false;
}
#endif

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

        if (custom_col_pins[col]&L_MASK) {
            gpio_write_pin(LEFT_EN_PIN,  0);
        }
        if (custom_col_pins[col]&R_MASK) {
            gpio_write_pin(RIGHT_EN_PIN, 0);
        }

        for (int row = 0; row < MATRIX_ROWS; row++) {
            matrix_row_t last_row_value    = raw[row];
            matrix_row_t current_row_value = last_row_value;

            if (sense_key(custom_row_pins[row], current_row_value&(1<<col), row, col)) {
                current_row_value |= (1 << col);
            } else {
                current_row_value &= ~(1 << col);
            }

            if (last_row_value != current_row_value) {
                raw[row] = current_row_value;
                changed = true;
                ec_matrix_dump_row(&ec_matrix, row);
            }
        }
        gpio_write_pin(LEFT_EN_PIN,  1);
        gpio_write_pin(RIGHT_EN_PIN, 1);
    }
#endif

#ifdef DIRECT_KEY_PIN
    changed |= check_direct_pin(raw);
#endif

    if (changed) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            custom_matrix_debug("row:%d-%x\n", row, matrix_get_row(row));
        }
    }
    return changed;
}

static void ec_matrix_init(ec_matrix_t *matrix)
{
    // currently useless, plan for storing calibrated data on eeprom and read out
}

static bool ec_auto_check(ec_key_t *key, uint16_t value) 
{
    bool key_down = false;

    if (key->max_auto > SENSE_TH) {
        if (value > ((key->min_auto+key->max_auto)*key->stroke)/STROKE_MAX) {
            key_down = true;
        }
    } else {
        if (value > SENSE_TH) {
            key_down = true;
        }
    }
    return key_down;
}

static bool ec_matrix_update(ec_matrix_t *matrix, uint8_t row, uint8_t col, uint16_t value)
{
    ec_key_t *key = &matrix->keys[row][col];

    key->real = value;

    if (key->min_auto > value) {
        key->min_auto = value;
    }

    if (key->max_auto < value) {
        if (value > EC_MAX) {
            key->max_auto = EC_MAX;
            custom_matrix_debug("MAX hit row:%d,col:%d, value:%d\n", row, col, value);
        } else {
            key->max_auto = value;
        }
    }

    bool key_down = false;
    switch (matrix->mode) {
    case EC_DEFAULT_MODE:
        if (key->max_def == INVALID_MAX) {
            key_down = ec_auto_check(key, value);
        } else {
            if (value > ((key->min_def+key->max_def)*key->stroke)/STROKE_MAX) {
                key_down = true;
            }
        }
        break;
    case EC_AUTO_MODE:
    case EC_CALIBRATE_MODE:
        key_down = ec_auto_check(key, value);
        break;
    default:
        break;
    }

    return key_down;
}

static void ec_dump_key(ec_key_t *key, uint8_t row, uint8_t col)
{
#ifdef CDC_ENABLE
    void cdc_output(const char* buf, uint32_t size);
    static char buf[128];
#if 1   // dump for code
    int count = sprintf(buf, "row:%d,col:%d, {%d,%d,AUTO_MIN, AUTO_MAX, 0, STROKE_DEF},\n",
                            row, col, key->min_auto, key->max_auto);
#else
    int count = sprintf(buf, "row:%d,col:%d, min:%d, max:%d, value:%d, stoke:%d\n",
                        row, col, key->min_auto, key->max_auto, key->real, key->stroke);
#endif
    cdc_output(buf, count);

#endif
    custom_matrix_debug("row:%d,col:%d, min:%d, max:%d, value:%d, stoke:%d\n",
                        row, col, key->min_auto, key->max_auto, key->real, key->stroke);
}

static void ec_matrix_dump_row(ec_matrix_t *matrix, uint8_t row)
{
    if (matrix->mode != EC_CALIBRATE_MODE) return;

    //for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            ec_key_t *key = &matrix->keys[row][col];
            ec_dump_key(key, row, col);
        }
    //}
}
