/**
 * config.h
 *
 */

#pragma once

// USB Device descriptor parameter
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x5545   // UD
#define DEVICE_VER 0x0001
#define MANUFACTURER Matrix

#if defined(FALCON)
    #define PRODUCT Falcon
    #define MATRIX_ROWS 5
    #define MATRIX_COLS 14
    #define S1_PIN A6
    #define S2_PIN A7
    #define RESET_PIN S1_PIN
    #define VECT_TAB_OFFSET 0x10000
#elif defined(ABELX)
    #define PRODUCT Abelx
    #define MATRIX_ROWS 6
    #define MATRIX_COLS 16
    #define S1_PIN A7
    #define S2_PIN B0
    #define RESET_PIN S1_PIN
    #define VECT_TAB_OFFSET 0x10000
#else
#error "BOARD not defined"
#endif


#define VIAL_INSECURE
#define VIAL_KEYBOARD_UID {0xFD, 0x18, 0xB5, 0x4A, 0x99, 0x22, 0x34, 0xC9}

#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"
#define PROGMEM