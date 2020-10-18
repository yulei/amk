/**
 * config.h
 *
 */

#pragma once

// USB Device descriptor parameter
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x5044   // PD
#define DEVICE_VER 0x0001
#define MANUFACTURER matrix
#define PRODUCT M20Pad 

// key matrix
#define MATRIX_ROWS 6
#define MATRIX_COLS 4

#define MATRIX_DIRECT_PINS  { A10, B12,  A0,  A1}
#define MATRIX_ROW_PINS     {  B0,  C5,  C4,  A7,  A6}
#define MATRIX_COL_PINS     { B11, B10,  B1,  A2}
