/**
 * rgb_matrix.h
 *      rgb matrix definition
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "rgb_driver.h"

typedef struct {
    uint8_t enable;
    uint8_t mode;
    uint8_t speed;
    uint8_t hue;
    uint8_t sat;
    uint8_t val;
} rgb_matrix_config_t;

void rgb_matrix_init(rgb_driver_t *driver);
bool rgb_matrix_enabled(void);
void rgb_matrix_inc_hue(void);
void rgb_matrix_dec_hue(void);
void rgb_matrix_inc_sat(void);
void rgb_matrix_dec_sat(void);
void rgb_matrix_inc_val(void);
void rgb_matrix_dec_val(void);
void rgb_matrix_inc_speed(void);
void rgb_matrix_dec_speed(void);
void rgb_matrix_inc_mode(void);
void rgb_matrix_dec_mode(void);
void rgb_matrix_toggle(void);
void rgb_matrix_task(void);