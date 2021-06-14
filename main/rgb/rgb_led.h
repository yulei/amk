/**
 * @file rgb_led.h
 */

#pragma once

#include "rgb_driver.h"

void rgb_led_init(void);
void rgb_led_task(void);
void rgb_led_prepare_sleep(void);

rgb_driver_t *rgb_led_map(uint8_t led_index);

bool rgb_led_is_on(void);
void rgb_led_set_all(bool on);

void rgb_led_config_init(void);
void rgb_led_config_next(void);
bool rgb_led_config_enabled(void);
void rgb_led_config_toggle(void);

void rgb_led_config_inc_param(uint8_t param);
void rgb_led_config_dec_param(uint8_t param);
