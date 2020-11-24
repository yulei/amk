
#pragma once

#include <stdint.h>

/* bootmagic salt key */
#ifndef BM_KEY_SALT
#define BM_KEY_SALT             KC_SPACE
#endif

/* skip bootmagic and eeconfig */
#ifndef BM_KEY_SKIP
#define BM_KEY_SKIP             KC_B
#endif

/* eeprom clear */
#ifndef BM_KEY_EEPROM_CLEAR
#define BM_KEY_EEPROM_CLEAR     KC_BSPACE
#endif

/* kick up bootloader */
#ifndef BM_KEY_BOOTLOADER
#define BM_KEY_BOOTLOADER       KC_ESC
#endif

void bootmagic(void);
bool bootmagic_scan_key(uint16_t action);