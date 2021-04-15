/**
 * config.h
 *
 */

#pragma once

// USB Device descriptor parameter
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x0111   // one key
#define DEVICE_VER 0x0001
#define MANUFACTURER astro
#define PRODUCT One Key

// key matrix
#define MATRIX_ROWS 1
#define MATRIX_COLS 1

#define MATRIX_ROW_PINS { B15}
#define MATRIX_COL_PINS { C15}

//#define MATRIX_ROW_PINS { PA04}
//#define MATRIX_COL_PINS { PA05}

#define SCREEN_NUM      2

#define SCREEN_0_RESET  A0
#define SCREEN_0_CS     A1
#define SCREEN_0_DC     A2

#define SCREEN_1_RESET  B0
#define SCREEN_1_CS     B1
#define SCREEN_1_DC     B2

#define RGB_LED_NUM     4
#define WS2812_LED_PIN  A0