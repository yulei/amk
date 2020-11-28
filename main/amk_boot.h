
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