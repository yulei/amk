/**
 * config.h
 *
 */

#pragma once

#define VENDOR_ID       0x4D58   // MX
#define PRODUCT_ID      0x8202   // 8xv2
#define DEVICE_VER      0x0001
#define MANUFACTURER    Matrix Lab
#define PRODUCT         8XV2 OG HS

// key matrix
#define MATRIX_ROWS         6
#define MATRIX_COLS         16
#define MATRIX_ROW_PINS     {A15, B5, B6, A10, A7, B3}
#define MATRIX_COL_PINS     {C9, C8, C7, C6, C5, C4, C3, C2, C1, C0, C15, C14, C13, C12, C11, C10};

#define CAPS_LED_PIN        A8
#define SCROLL_LED_PIN      B4

#define USE_3731
#define USE_I2C1
#define I2C1_PINS   (GPIO_PIN_8 | GPIO_PIN_9)
#define I2C1_REMAP

#define RGB_LED_NUM         20
#define RGB_DEVICE_NUM      1
#define RGB_SEGMENT_NUM     2


#define DEBOUNCE            5
#define DWT_DELAY
#define HSE_VALUE           16000000
#define VECT_TAB_OFFSET     0x10000
#define SUSPEND_RESET

// vial 
#define VIAL_KEYBOARD_UID {0x48, 0x90, 0x3E, 0x8A, 0x84, 0x73, 0x81, 0x4F}

// custom tusb definitions
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

//#define USE_HS_USB

#ifdef USE_HS_USB
#define DEBOUNCE        1
#endif