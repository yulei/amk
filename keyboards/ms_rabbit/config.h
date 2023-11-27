/**
 * config.h
 *
 */
#pragma once

#define VENDOR_ID       0x4D47      // MG
#define PRODUCT_ID      0x4852      // HR
#define DEVICE_VER      0x0001
#define MANUFACTURER    Matrix Lab
#define PRODUCT         Rabbit MS

#define MATRIX_ROWS     4
#define MATRIX_COLS     16

#define MATRIX_ROW_PINS {A9, A10, C11, A8}
#define MATRIX_COL_PINS {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

#define MAG_ZERO        2048
#define MAG_TH_LOW      2200
#define MAG_TH_HIGH     2600

#define COL_A_PIN       A15
#define COL_B_PIN       C10
#define COL_C_PIN       C12
#define COL_D_PIN       B3

#define USE_ADC1
#define ADC_INST        hadc1
#define KEY_IN_CHANNEL  ADC_CHANNEL_1
#define KEY_IN_PIN      GPIO_PIN_1
#define KEY_IN_PORT     GPIOA

// rgb led
#define RGB_DEVICE_NUM  2

#define USE_3733
#define RGB_MATRIX_LED_NUM     63
#define RGB_MATRIX_LED_COUNT    RGB_MATRIX_LED_NUM
#define RGB_MATRIX_NUM      1

#define USE_3236
#define RGB_SEGMENT_NUM 1

#define RGB_LED_NUM     (3 + RGB_MATRIX_LED_NUM)

#define USE_I2C1
#define I2C1_SCL_PIN    B6
#define I2C1_SDA_PIN    B7

#define DEBOUNCE        0

#define DWT_DELAY

#define VECT_TAB_OFFSET 0x20000
#define HSE_VALUE       16000000

#define VIAL_KEYBOARD_UID {0x33, 0x83, 0x29, 0x0D, 0xA3, 0xCA, 0xDB, 0x94}
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

#define RABBIT_MS_V2

#define APC_INTERVAL_MIN            200
#define DYNAMIC_KEYMAP_LAYER_COUNT  7
#define APC_KEYMAP_DOWN_LAYER       4
#define APC_KEYMAP_UP_LAYER         5
#define APC_KEYMAP_DKS_LAYER        6

#define USE_HS_USB
#define SUSPEND_RESET
