/**
 * Copyright 2021 astro. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

/**
 * @file amk_boot.h
 */

#pragma once

#include <stdint.h>

/* salt key */
#ifndef BM_KEY_SALT
#define BM_KEY_SALT             KC_SPACE
#endif

/* eeprom clear */
#ifndef BM_KEY_EEPROM_CLEAR
#define BM_KEY_EEPROM_CLEAR     KC_BSPACE
#endif

void pre_boot_init(void);
void boot_init(void);
void post_boot_init(void);