/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

/**
 * @file amk_eeprom.h
 */

#pragma once

#include "eeconfig.h"

#define EECONFIG_RGB_MAX_COUNT      8       // support rgb maximum as 8
#define EECONFIG_RGB_SIZE           6       // one rgb need 6 bytes storage
#define EECONFIG_RGB                (uint8_t*)(EEPROM_SIZE)
#define AMK_EEPROM_SIZE             (EEPROM_SIZE+EECONFIG_RGB_MAX_COUNT*EECONFIG_RGB_SIZE)

#ifdef RGB_ENABLE
void eeconfig_read_rgb(void* rgb, uint8_t index);
void eeconfig_write_rgb(const void* rgb, uint8_t index);
void eeconfig_update_rgb(const void* rgb, uint8_t index);
#endif
