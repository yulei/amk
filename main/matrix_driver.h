/**
 * matrix_driver.h
 * common matrix interface
 */

#pragma once

#include "matrix.h"

typedef void (*matrix_event_callback_f)(bool changed);

typedef void (*matrix_trigger_start_f)(matrix_event_callback_f event_cb);
typedef void (*matrix_trigger_stop_f)(void);

typedef void (*matrix_scan_start_f)(void);
typedef void (*matrix_scan_stop_f)(void);

typedef void (*matrix_prepare_sleep_f)(void);

typedef struct {
    matrix_trigger_start_f  trigger_start;
    matrix_trigger_stop_f   trigger_stop;
    matrix_scan_start_f     scan_start;
    matrix_scan_stop_f      scan_stop;
    matrix_prepare_sleep_f  prepare_sleep;
    matrix_event_callback_f event_callback;
    bool                    trigger_mode;
} matrix_driver_t;

extern matrix_driver_t matrix_driver;

void matrix_driver_init(void);
