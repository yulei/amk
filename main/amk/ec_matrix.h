/**
 * @file ec_matrix.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdbool.h>
#include "amk_gpio.h"

#define EC_INVALID_MAX  0xFFFF
#define EC_AUTO_MIN     0xFFFF
#define EC_AUTO_MAX     0x0

#ifndef EC_SENSE_MAX
#define EC_SENSE_MAX    2560
#endif

#define STROKE_MIN      8
#define STROKE_MAX      20

#ifndef STROKE_DEFAULT
#define STROKE_DEFAULT  14
#endif

#ifndef DISCHARGE_WAIT_PRE
#define DISCHARGE_WAIT_PRE  25
#endif

#ifndef DISCHARGE_WAIT_POST
#define DISCHARGE_WAIT_POST 25
#endif

#ifndef DISCHARGE_DELAY
#define DISCHARGE_DELAY     2
#endif

#ifndef DISCHARGE_ENABLE
#define DISCHARGE_ENABLE    1
#endif

#ifndef DISCHARGE_DISABLE   
#define DISCHARGE_DISABLE   0
#endif

#define COL_A_MASK  0x01
#define COL_B_MASK  0x02
#define COL_C_MASK  0x04

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

extern ec_matrix_t ec_matrix;

void ec_matrix_init(ec_matrix_t *matrix);
bool ec_matrix_sense(pin_t row_pin, uint8_t row, uint8_t col);
void ec_matrix_dump_row(ec_matrix_t *matrix, uint8_t row);
