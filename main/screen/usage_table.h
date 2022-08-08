/**
 * @file usage_table.h
 * @author astro
 * @brief convert keyboard usage to character
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

char usage2char(uint8_t keycode, bool shift, bool caps);
