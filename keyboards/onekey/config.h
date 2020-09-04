/**
 * config.h
 *
 */

#pragma once

// USB Device descriptor parameter
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x0101   // one key
#define DEVICE_VER 0x0001
#define MANUFACTURER astro
#define PRODUCT One Key
#define DESCRIPTION a keybaord for testing

// key matrix
#define MATRIX_ROWS 1
#define MATRIX_COLS 1

#define MATRIX_ROW_PINS {  B0}
#define MATRIX_COL_PINS { C15}