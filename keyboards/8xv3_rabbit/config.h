/**
 * config.h
 *
 */

#pragma once

#define VENDOR_ID       0x4D58   // MX
#define PRODUCT_ID      0x3858   // 8X
#define DEVICE_VER      0x0003
#define MANUFACTURER    Matrix Lab
#define PRODUCT         8XV3.0 Rabbit

// key matrix
#define MATRIX_ROWS         6
#define MATRIX_COLS         16
#define MATRIX_ROW_PINS     {B10, A0, C15, C14, C13, B4}

#define LEFT_MASK   0x08
#define RIGHT_MASK  0x10
#define MATRIX_COL_PINS     {LEFT_MASK|5, LEFT_MASK|7, LEFT_MASK|6, LEFT_MASK|4, LEFT_MASK|2, LEFT_MASK|1, LEFT_MASK|0, LEFT_MASK|3, RIGHT_MASK|5, RIGHT_MASK|7, RIGHT_MASK|6, RIGHT_MASK|4, RIGHT_MASK|2, RIGHT_MASK|1, RIGHT_MASK|0, RIGHT_MASK|3};

#define LEFT_EN_PIN         B2
#define RIGHT_EN_PIN        A2
#define COL_A_PIN           A3
#define COL_B_PIN           B0
#define COL_C_PIN           B1

#define KEY_PIN             A1

#define USE_3741
#define USE_3236

#define USE_I2C1
#define IS31FL3741_I2C_ID   I2C_INSTANCE_1
#define IS31FL3236_I2C_ID   I2C_INSTANCE_1

#define RGB_MATRIX_LED_NUM  97
#define RGB_MATRIX_NUM      1

#define IS31FL3236_LED_NUM  6

#define RGB_LED_NUM         (IS31FL3236_LED_NUM+RGB_MATRIX_LED_NUM)
#define RGB_DEVICE_NUM      2
#define RGB_SEGMENT_NUM     2

#define DEBOUNCE            5
#define DWT_DELAY
#define VECT_TAB_OFFSET     0x20000
#define SUSPEND_RECONNECT
//#define FAULT_BREAK

// vial 
#define VIAL_KEYBOARD_UID {0xD0, 0xD0, 0x10, 0x8F, 0x9D, 0x7A, 0x29, 0x92}

// custom tusb definitions
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"
