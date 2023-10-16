/**
 * config.h
 *
 */
#pragma once

#define VENDOR_ID       0x4D58      // MX
#define PRODUCT_ID      0x4852      // HR
#define DEVICE_VER      0x0001
#define MANUFACTURER    Matrix
#define PRODUCT         Magic3 60

#define MATRIX_ROWS     5
#define MATRIX_COLS     16
#define MATRIX_ROW_PINS {B15,B14,B12,A4,B5}
#define MATRIX_COL_PINS {C9,C8,C7,C6,C5,C4,C3,C2,C1,C0,C15,C14,C13,C12,C11,C10}

#define CAPS_LED_PIN    B13
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

// rgb led
#define USE_3236
#define RGB_LED_NUM     3
#define RGB_SEGMENT_NUM 1
#define RGB_DEVICE_NUM  1

#define USE_I2C1
#define I2C1_SCL_PIN    B6
#define I2C1_SDA_PIN    B7

// vial
//#define VIAL_INSECURE
#define VIAL_KEYBOARD_UID {0x09, 0x2A, 0xF3, 0x2B, 0x6D, 0xDB, 0x25, 0xEF}

//
#define DWT_DELAY
#define VECT_TAB_OFFSET     0x10000
#define SUSPEND_RECONNECT

//#define USE_HS_USB

#ifdef USE_HS_USB
#define DEBOUNCE        1
#endif


