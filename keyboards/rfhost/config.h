/**
 * config.h
 *
 */
#pragma once

/* USB Device descriptor parameter */
#define VENDOR_ID       0x4D58   // MX
#define PRODUCT_ID      0x5246   // RF
#define DEVICE_VER      0x0001
#define MANUFACTURER    astro
#define PRODUCT         RF host

/* key matrix size */
#define MATRIX_ROWS 1
#define MATRIX_COLS 1

#define MATRIX_ROW_PINS {12}
#define MATRIX_COL_PINS {14}

// rf host do not need sleep
#define GZLL_IS_HOST 1
#define DISABLE_SLEEP

#define VIAL_KEYBOARD_UID {0x1C, 0xCD, 0x4E, 0xDE, 0x6B, 0x70, 0x00, 0xAE}