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

#define MATRIX_ROW_PINS { B0}
#define MATRIX_COL_PINS { B1}

#define SCREEN_NUM      2

#define SCREEN_0_RESET  A0
#define SCREEN_0_CS     A1
#define SCREEN_0_DC     A2

#define SCREEN_1_RESET  B0
#define SCREEN_1_CS     B1
#define SCREEN_1_DC     B2

#if 0
#define WS2812_LED_NUM  4
#define WS2812_LED_PIN  A0
#define PWM_TIM_PERIOD  134
#define PWM_TIM htim2
#define PWM_TIM_CHANNEL TIM_CHANNEL_1
#endif

#define WS2812_LED_NUM  4
#define WS2812_LED_PIN  A6
#define PWM_TIM_PERIOD  39
#define PWM_TIM htim3
#define PWM_TIM_CHANNEL TIM_CHANNEL_1
//#define WS2812_1        27
//#define WS2812_0        12


//#define WAIT_TIM      htim2

#define RGB_DEVICE_NUM  1
#define RGB_SEGMENT_NUM 1
#define RGB_LED_NUM     4

//#define RGB_DEVICE_NUM  2
//#define USE_I2C1
//#define I2C1_SCL_PIN    B8
//#define I2C1_SDA_PIN    B9

//#define RGB_SEGMENT_NUM 2
//#define RGB_LED_NUM     15
//#define RGB_SEGMENT_NUM 3
//#define RGB_LED_NUM     24

//#define VECT_TAB_OFFSET 0x20000

#define XTAL_VALUE      16000000
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"
//#define CFG_TUSB_CONFIG_FILE    "tusb_def_host.h"
