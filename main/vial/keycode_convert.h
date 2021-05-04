/**
 * @file keycode_convert.h
 * 
 * Copyright 2021 astro 
 */

#pragma once

#include <stdint.h>
/**
 * @brief convert keycode from amk to vial
 */
uint16_t amk_to_vial(uint16_t keycode);

/**
 * @brief convert keycode from vial to amk
 */
uint16_t vial_to_amk(uint16_t keycode);