/**
 * config.h
 *
 */

#pragma once

// USB Device descriptor parameter
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x0065   // noah 65
#define DEVICE_VER 0x0001
#define MANUFACTURER matrix
#define PRODUCT NOAH
#define DESCRIPTION 65% keybaord of the Matrix abel family

// key matrix
#define MATRIX_ROWS 5
#define MATRIX_COLS 15

#define MATRIX_ROW_PINS {  B0,  A1, C14, C13,  A0}
#define MATRIX_COL_PINS { C15, B10,  B7,  B6,  B5,  B4, A15, A10,  A9,  A8, B15, B14, B13, B12,  B2}


#define RGB_LED_NUM     4
#define WS2812_LED_PIN  B1