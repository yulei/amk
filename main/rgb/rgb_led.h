/**
 * @file rgb_led.h
 */

#pragma once

#include "rgb_driver.h"

void rgb_led_init(void);
void rgb_led_task(void);

rgb_driver_t *rgb_led_map(uint8_t led_index);

void rgb_led_config_init(void);

void rgb_led_config_next(void);
bool rgb_led_config_enabled(void);
void rgb_led_config_toggle(void);

void rgb_led_config_inc_param(uint8_t param);
void rgb_led_config_dec_param(uint8_t param);
