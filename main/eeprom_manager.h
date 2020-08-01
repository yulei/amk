/**
 * eeprom_manager.h
 */

#pragma once

#include <stdint.h>

#include "eeconfig.h"

#define EECONFIG_KEYBOARD   (uint32_t*)16

uint32_t eeconfig_read_kb(void);
void eeconfig_write_kb(uint32_t data);
void eeconfig_update_kb(uint32_t data);