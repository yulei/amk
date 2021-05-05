/**
 * config.h
 *
 */

#pragma once

// USB Device descriptor parameter
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x5545   // UD
#define DEVICE_VER 0x0001
#define MANUFACTURER astro
#define PRODUCT HHKB USB

// key matrix
#define MATRIX_ROWS 1
#define MATRIX_COLS 1

#define MATRIX_ROW_PINS { B15}
#define MATRIX_COL_PINS { B1}

#define S1_PIN A6
#define S2_PIN A7

#define RESET_PIN A6
#define VECT_TAB_OFFSET 0x10000

#define VIAL_INSECURE
#define VIAL_KEYBOARD_UID {0xFD, 0x18, 0xB5, 0x4A, 0x99, 0x22, 0x34, 0xC9}

#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"
#define PROGMEM