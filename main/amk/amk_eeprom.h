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

#define EECONFIG_RGB_MAX            7       // support max rgb segments as 7
#define EECONFIG_RGB_SIZE           6       // one rgb need 6 bytes storage
#define EECONFIG_RGB_COUNT          (EECONFIG_RGB_SIZE*EECONFIG_RGB_MAX)
#define EECONFIG_RGB                (uint8_t*)(EEPROM_SIZE)
#define EECONFIG_USB                (uint8_t*)(EEPROM_SIZE+EECONFIG_RGB_COUNT)
#define EECONFIG_USB_COUNT          1
#define EECONFIG_DEBOUNCE           (uint8_t*)(EEPROM_SIZE+EECONFIG_RGB_COUNT+1)
#define EECONFIG_DEBOUNCE_COUNT     1
#define EECONFIG_REMAIN_COUNT       6
#define EECONFIG_RESERVED_COUNT     2048
#define AMK_EEPROM_SIZE             (EEPROM_SIZE+EECONFIG_RGB_COUNT+EECONFIG_USB_COUNT+EECONFIG_DEBOUNCE_COUNT+EECONFIG_REMAIN_COUNT+EECONFIG_RESERVED_COUNT)

#ifdef RGB_ENABLE
void eeconfig_read_rgb(void* rgb, uint8_t index);
void eeconfig_write_rgb(const void* rgb, uint8_t index);
void eeconfig_update_rgb(const void* rgb, uint8_t index);
#endif

#ifdef USE_HS_USB
uint8_t eeconfig_read_usb(void);
void eeconfig_update_usb(uint8_t usb);
#endif

#ifdef STATE_SCAN_ENABLE
uint8_t eeconfig_read_debounce(void);
void eeconfig_update_debounce(uint8_t debounce);
#endif