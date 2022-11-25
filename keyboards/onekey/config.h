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
#define MATRIX_ROWS 2
#define MATRIX_COLS 3

#define MATRIX_ROW_PINS {A8,A9}
#define MATRIX_COL_PINS {C6,C7,C8}

// screen configuration
#if 0
#define USE_SPI1
#define GC9107_SPI_ID       SPI_INSTANCE_1
#define SCREEN_NUM          1
#define SCREEN_0_PWR        C5
#define SCREEN_0_PWR_EN     0
#define SCREEN_0_CS         A4
#define SCREEN_0_RESET      C4
#define SCREEN_0_DC         A3
#endif

#define USE_SPI1
#define ST7735_SPI_ID       SPI_INSTANCE_1
#define SCREEN_NUM          1
#define SCREEN_0_PWR        C12
#define SCREEN_0_PWR_EN     0
#define SCREEN_0_CS         D2
#define SCREEN_0_RESET      B7
#define SCREEN_0_DC         B6

#define SCREEN_ROTATION     1

#if 0
#define WS2812_LED_NUM  4
#define WS2812_LED_PIN  B7
#define PWM_TIM_PERIOD  209
#define PWM_TIM TIMER3 
#define PWM_TIM_CHANNEL TIMER_CH_1
#endif

#if 0
#define WS2812_LED_NUM  4
#define WS2812_LED_PIN  A6
#define PWM_TIM_PERIOD  39
#define PWM_TIM htim3
#define PWM_TIM_CHANNEL TIM_CHANNEL_1
//#define WS2812_1        27
//#define WS2812_0        12


//#define WAIT_TIM      htim2
#endif

#define DWT_DELAY

//#define RGB_LED_NUM     4
//#define RGB_DEVICE_NUM  2
//#define RGB_SEGMENT_NUM 2

//#define RGB_DEVICE_NUM  2
//#define USE_I2C1
#define I2C1_SCL_PIN    B8
#define I2C1_SDA_PIN    B9

//#define RGB_LED_NUM     16
#define RGB_LED_NUM     12
#define RGB_SEGMENT_NUM 1
#define RGB_DEVICE_NUM  1
//#define RGB_LED_NUM     24

//#define VECT_TAB_OFFSET 0x20000

//#define AMK_PROFILING
//#define XTAL_VALUE      16000000
//#define HXTAL_VALUE     16000000
//#define HSE_VALUE      HXTAL_VALUE //16000000
//#define HSE_VALUE      12000000
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"
//#define CFG_TUSB_CONFIG_FILE    "tusb_def_host.h"
