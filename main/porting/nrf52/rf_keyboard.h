/**
 * @file rf_keyboard.h
 * 
 */

#pragma once

#include "common_config.h"

void rf_keyboard_init(rf_send_report_t send_report, rf_prepare_sleep_t prepare_sleep);

void rf_keyboard_start(void);

void rf_keyboard_prepare_sleep(void);

void rf_keyboard_jump_bootloader(void);
