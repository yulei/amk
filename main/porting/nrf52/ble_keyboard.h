/**
 * @file ble_keybaord.h
 * @brief keyboard implementation
 */

#pragma once

#include "host.h"

void ble_keyboard_init(void);
void ble_keyboard_start(void);
void ble_keyboard_sleep_prepare(void);
void ble_keyboard_jump_bootloader(void);
