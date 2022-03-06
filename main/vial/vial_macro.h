/**
 * @file vial_macro.h
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief to avoid report queue overflow, process only one keycode
 * @return false means finished
 */
bool vial_macro_play(uint8_t id, uint16_t *offset);

void vial_update_tri_layer(uint8_t layer1, uint8_t layer2, uint8_t layer3);
