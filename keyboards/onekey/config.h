/**
 * config.h
 *
 */

#pragma once

// USB Device descriptor parameter
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x0111   // one key
#define DEVICE_VER 0x0001
#define MANUFACTURER astro
#define PRODUCT One Key

// key matrix
#define MATRIX_ROWS 1
#define MATRIX_COLS 1

#define MATRIX_ROW_PINS { C0}
#define MATRIX_COL_PINS { B0}

#define SCREEN_NUM      2

#define SCREEN_0_RESET  A0
#define SCREEN_0_CS     A1
#define SCREEN_0_DC     A2

#define SCREEN_1_RESET  B0
#define SCREEN_1_CS     B1
#define SCREEN_1_DC     B2

#define WS2812_LED_NUM  4
#define WS2812_LED_PIN  A0
#define PWM_TIM_PERIOD  134
#define PWM_TIM htim2
#define PWM_TIM_CHANNEL TIM_CHANNEL_1

#define RGB_DEVICE_NUM  2
#define I2C_USE_INSTANCE_1
#define I2C1_SCL_PIN    B8
#define I2C1_SDA_PIN    B9

//#define RGB_SEGMENT_NUM 2
//#define RGB_LED_NUM     11
#define RGB_SEGMENT_NUM 3
#define RGB_LED_NUM     24

#define VECT_TAB_OFFSET 0x20000