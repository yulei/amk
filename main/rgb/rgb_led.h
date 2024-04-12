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
bool rgb_led_config_is_global(void);
void rgb_led_config_set_global(bool on);
void rgb_led_config_toggle(void);
void rgb_led_config_toggle_temp(void);
void rgb_led_config_inc_param(uint8_t param);
void rgb_led_config_dec_param(uint8_t param);

uint8_t rgb_led_strip_get_mode(uint8_t index);
void rgb_led_strip_set_mode(uint8_t index, uint8_t mode);
void rgb_led_strip_get_led(uint8_t index, uint8_t* hue, uint8_t* sat, uint8_t* val, uint8_t* param);
void rgb_led_strip_set_led(uint8_t index, uint8_t hue, uint8_t sat, uint8_t val, uint8_t param);
