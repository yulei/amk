/**
 * config.h
 *
 */

#pragma once

#define VENDOR_ID       0x4D58   // MX
#define PRODUCT_ID      0xAC54   // ACTION
#define DEVICE_VER      0x0001
#define MANUFACTURER    Matrix Lab
#define PRODUCT         Navi

// key matrix
#define MATRIX_ROWS         5
#define MATRIX_COLS         17

#define PROD
#ifdef PROD

#define MATRIX_ROW_PINS     {C13, C7, C6, C1, C0}
#define MATRIX_COL_PINS     {A0, A1, A2, B10, B2, B0, A3, A4, A5, B12, B13, B14, B15, C8, C14, C2, C15}

#define CAPS_LED_PIN        C3
#else
#if 0
#define MATRIX_ROW_PINS     {B14, A5, B13, C8, A4}
#define MATRIX_COL_PINS     {C13, C14, C15, C0, C1, C2, C3, A0, A1, A2, B10, B2, B0, A3, C7, C6, B15}

#define CAPS_LED_PIN        B12
#else
#define MATRIX_ROW_PINS     {C13, C7, A0, C1, C0}
#define MATRIX_COL_PINS     {C6, B15, B14, B13, B12, A5, A4, A3, B0, B2, B10, A2, A1, C8, C14, C2, C15}

#define CAPS_LED_PIN        C3
#endif
#endif

#define USE_3236

#define USE_I2C1
//#define IS31FL3236          I2C_INSTANCE_1
#define I2C1_SCL_PIN        B8
#define I2C1_SDA_PIN        B9

#define RGB_DEVICE_NUM      1
#define RGB_LED_NUM         3
#define RGB_SEGMENT_NUM     1

#define DEBOUNCE            5
#define DWT_DELAY
#define VECT_TAB_OFFSET     0x20000
#define HSE_VALUE           16000000U
//#define FAULT_BREAK

#define USE_SPI1
#define ST7735_SPI_ID       SPI_INSTANCE_1
#define SCREEN_NUM          1
#define SCREEN_0_PWR        C12
#define SCREEN_0_PWR_EN     0
#define SCREEN_0_CS         D2
#define SCREEN_0_RESET      B7
#define SCREEN_0_DC         B6
#define GC9107_AS_ST7735

#define SCREEN_ROTATION     3

// vial 
#define VIAL_KEYBOARD_UID {0x21, 0x8B, 0xFE, 0xA4, 0xE9, 0x5F, 0x65, 0x1B}

// custom tusb definitions
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

#define SUSPEND_RESET
//#define FAULT_BREAK

//#define RGB_INDICATOR_NUM   2
