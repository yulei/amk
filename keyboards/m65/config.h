/**
 * config.h
 *
 */

#pragma once

// USB Device descriptor parameter
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x4D65   // M65 with screen
#define DEVICE_VER 0x0001
#define MANUFACTURER Matrix Lab
#define PRODUCT Matrix 65 with Screen
#define DESCRIPTION 65% keybaord of the Matrix abel family

// key matrix
#define MATRIX_ROWS 5
#define MATRIX_COLS 15

#define MATRIX_ROW_PINS {  A0, C15, C14, B10,  B1}
#define MATRIX_COL_PINS { A10,  A9,  A8, B15, B14, B13,  B2, B12,  B7,  B6,  B5,  B4, A15, C13,  B0}

#define SCREEN_NUM      1
#define SCREEN_0_CS     A2
#define SCREEN_0_RESET  A3
#define SCREEN_0_DC     A4