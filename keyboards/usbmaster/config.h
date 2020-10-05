/**
 * config.h
 *
 */

#pragma once

// USB Device descriptor parameter
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x5543   // UC
#define DEVICE_VER 0x0001
#define MANUFACTURER astro
#define PRODUCT USB Controller 

// key matrix
#define MATRIX_ROWS 1
#define MATRIX_COLS 1

#define MATRIX_ROW_PINS { B15}
#define MATRIX_COL_PINS { B1}

#define S1_PIN A6
#define S2_PIN A7

#define RESET_PIN A6