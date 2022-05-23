/**
 * @file ec_matrix.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "ec_matrix.h"
#include "wait.h"
#include "amk_printf.h"

#ifndef EC_MATRIX_DEBUG
#define EC_MATRIX_DEBUG 0
#endif

#if EC_MATRIX_DEBUG
#define ec_matrix_debug  amk_printf
#else
#define ec_matrix_debug(...)
#endif

void ec_matrix_init(ec_matrix_t *matrix)
{

}

static bool ec_key_down(uint16_t value, bool on, uint16_t release, uint16_t press)
{
    if (on) {
        // press to release
        if (value > release) {
            return true;
        }
    } else {
        // release to press
        if (value > press) {
            return true;
        }
    }
    return false;
}

static bool ec_auto_check(ec_key_t *key, uint16_t value, bool on) 
{
    bool key_down = false;

    if (key->max_auto > EC_TH_HIGH) {
        key_down = ec_key_down(value, on, ((key->min_auto+key->max_auto)*key->release)/STROKE_MAX,
                                            ((key->min_auto+key->max_auto)*key->press)/STROKE_MAX);
    } else {
        key_down = ec_key_down(value, on, EC_TH_LOW, EC_TH_HIGH);
    }
    return key_down;
}

static bool ec_default_check(ec_key_t *key, uint16_t value, bool on) 
{
    bool key_down = false;

    key_down = ec_key_down(value, on, ((key->min_def+key->max_def)*key->release)/STROKE_MAX,
                                            ((key->min_def+key->max_def)*key->press)/STROKE_MAX);
    return key_down;
}

static bool ec_matrix_update(ec_matrix_t *matrix, bool on, uint8_t row, uint8_t col, uint16_t value)
{
    ec_key_t *key = &matrix->keys[row][col];

    key->real = value;

    if (key->min_auto > value) {
        key->min_auto = value;
    }

    if (key->max_auto < value) {
        if (value > EC_SENSE_MAX) {
            // drop it
            key->max_auto = EC_SENSE_MAX;
            ec_matrix_debug("MAX hit row:%d,col:%d, value:%d\n", row, col, value);
        } else {
            key->max_auto = value;
        }
    }

    bool key_down = false;
    switch (matrix->mode) {
    case EC_DEFAULT_MODE:
        if (key->max_def == EC_INVALID_MAX) {
            key_down = ec_auto_check(key, value, on);
        } else {
            key_down = ec_default_check(key, value, on);
        }
        break;
    case EC_AUTO_MODE:
    case EC_CALIBRATE_MODE:
        key_down = ec_auto_check(key, value, on);
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
    int count = sprintf(buf, "row:%d,col:%d, {%d,%d,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_PRESS, STOKE_RELEASE},\n",
                            row, col, key->min_auto, key->max_auto);
#else
    int count = sprintf(buf, "row:%d,col:%d, min:%d, max:%d, value:%d, stoke:%d\n",
                        row, col, key->min_auto, key->max_auto, key->real, key->stroke);
#endif
    cdc_output(buf, count);

#endif
    ec_matrix_debug("row:%d,col:%d, min:%d, max:%d, value:%d, stoke:%d\n",
                        row, col, key->min_auto, key->max_auto, key->real, key->stroke);
}

extern ADC_HandleTypeDef ADC_INST;

static uint32_t adc_read(void)
{
    uint32_t data = 0;

    __disable_irq();
    if (HAL_ADC_Start(&ADC_INST) == HAL_OK) {
        HAL_ADC_PollForConversion(&ADC_INST, 1);
        if ((HAL_ADC_GetState(&ADC_INST) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC)
        {
            data = HAL_ADC_GetValue(&ADC_INST);
        } else {
            amk_printf("key sense failed\n");
        }
        HAL_ADC_Stop(&ADC_INST);
    }
    __enable_irq();

#if 0
    if (HAL_ADC_Start(&hadc1) == HAL_OK) {
        HAL_ADC_PollForConversion(&hadc1, 1);
        if ((HAL_ADC_GetState(&hadc1) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC)
        {
            data += HAL_ADC_GetValue(&hadc1);
        } else {
            ec_matrix_debug("key sense failed\n");
        }
        HAL_ADC_Stop(&hadc1);
    }
#endif

    return data;
}

bool ec_matrix_sense(pin_t row_pin, uint8_t row, uint8_t col, bool on)
{
    bool key_down = false;
    gpio_write_pin(DISCHARGE_PIN, DISCHARGE_ENABLE);
    wait_us(DISCHARGE_WAIT_PRE);
    gpio_write_pin(DISCHARGE_PIN, DISCHARGE_DISABLE);
    wait_us(DISCHARGE_DELAY);
    gpio_write_pin(row_pin, 1);
    #ifdef CHARGE_WAIT
    wait_us(CHARGE_WAIT);
    #endif
    
    uint32_t data = adc_read();

    key_down = ec_matrix_update(&ec_matrix, on, row, col, data);

    gpio_write_pin(row_pin, 0);
    gpio_write_pin(DISCHARGE_PIN, DISCHARGE_ENABLE);
    wait_us(DISCHARGE_WAIT_POST);
    return key_down;
}

void ec_matrix_dump_row(ec_matrix_t *matrix, uint8_t row)
{
    if (matrix->mode != EC_CALIBRATE_MODE) return;

    //for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            ec_key_t *key = &matrix->keys[row][col];
            ec_dump_key(key, row, col);
        }
    //}
}
