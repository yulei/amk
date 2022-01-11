/**
 * config.h
 *
 */

#pragma once

// USB Device descriptor parameter
#define VENDOR_ID       0x4D58   // MX
#define PRODUCT_ID      0x6505   // M65 with screen
#define DEVICE_VER      0x0001
#define MANUFACTURER    Matrix Lab
#define PRODUCT         Vita

// key matrix
#define MATRIX_ROWS     5
#define MATRIX_COLS     15

#define MATRIX_ROW_PINS { A1, A15, D2, C12, B1}
#define MATRIX_COL_PINS { A8, A10, C9, C8, C7, C6, B7, B6, B5, B4, C10, B10, B11, B2, B0}

//#define CAPS_LED_PIN    A10

#define USE_SPI1
#define SCREEN_NUM      1
#define SCREEN_0_PWR    C0
#define SCREEN_0_PWR_EN 0
#define SCREEN_0_CS     A2
#define SCREEN_0_RESET  A3
#define SCREEN_0_DC     A4

#define USE_SPI2
#define FLASH_CS        B12
#define W25QXX_SPI_ID   SPI_INSTANCE_2

// custom tusb definitions
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

#define VECT_TAB_OFFSET 0x20000

#define I2C_USE_INSTANCE_1
#define I2C1_SCL_PIN    B8
#define I2C1_SDA_PIN    B9

#define WS2812_LED_NUM  5
#define WS2812_LED_PIN  A0
#define PWM_TIM_PERIOD  104
#define PWM_TIM htim2
#define PWM_TIM_CHANNEL TIM_CHANNEL_1

#define RGB_LED_NUM             5
#define RGB_DEVICE_NUM          1
#define RGB_SEGMENT_NUM         1
#define RGB_INDICATOR_LED_NUM   1

//#define VIAL_INSECURE
//#define VIAL_KEYBOARD_UID {0x01, 0x90, 0x17, 0x3A, 0xEC, 0xEE, 0x2B, 0x85}

//#define WDT_ENABLED
#define FAULT_BREAK
