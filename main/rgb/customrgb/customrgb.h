/**
 * @file customrgb.h
 * @author astro
 * 
 * @copyright Copyright (c) 2024
*/
#pragma once

#include <stdint.h>

void customrgb_init(void);
void customrgb_get_matrix_info(uint8_t index, uint8_t* start, uint8_t* count);
uint8_t customrgb_get_matrix_mode(uint8_t index, uint8_t* custom, uint8_t* count, uint8_t* def);

uint8_t customrgb_get_matrix_led_index(uint8_t index, uint8_t row, uint8_t col);
void customrgb_get_led(uint8_t index, uint8_t* hue, uint8_t* sat, uint8_t* val, uint8_t* param);
void customrgb_set_led(uint8_t index, uint8_t hue, uint8_t sat, uint8_t val, uint8_t param);
