/**
 * matrix_driver.h
 * common matrix interface
 */

#pragma once

#include "matrix.h"

typedef void (*matrix_event_callback)(bool changed);

void matrix_driver_init(void);
void matrix_driver_trigger_start(matrix_event_callback event_cb);
void matrix_driver_trigger_stop(void);
void matrix_driver_scan_start(void);
void matrix_driver_scan_stop(void);
void matrix_driver_prepare_sleep(void);

bool matrix_driver_keys_off(void);