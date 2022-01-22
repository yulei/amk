/**
 * config.h
 *
 */
#pragma once

/* USB Device descriptor parameter */
#define VENDOR_ID       0x4D58      // MX
#define PRODUCT_ID      0xAD20      
#define DEVICE_VER      0x0001
#define MANUFACTURER    astro
#define PRODUCT         Matrix 2.0 Add 

/* key matrix size */
#define MATRIX_ROWS     6
#define MATRIX_COLS     16

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE                    5

#define VBUS_DETECT_PIN             4
#define UART_RX_PIN                 20
#define UART_TX_PIN                 19
#define BATTERY_SAADC_ENABLE_PIN    13
#define BATTERY_SAADC_PIN           NRF_SAADC_INPUT_AIN3

#define USE_I2C1
#define I2C1_SCL_PIN                7
#define I2C1_SDA_PIN                8

#define RGBLIGHT_EN_PIN             9
#define RGBLIGHT_EN_HIGH
#define INDICATOR_SHDN_PIN          10

#define RGB_LED_NUM                 24

#define RGB_DEVICE_NUM              3
#define RGB_SEGMENT_NUM             2
#define RGB_INDICATOR_LED_NUM       4

#define WS2812_LED_NUM              4
#define WS2812_LED_PIN              12

#define WDT_ENABLE                  1        
//#define GZLL_IS_CLIENT              1
//#define DISABLE_USB
//#define SLEEP_COUNT_MAX             2
//#define RESET_ON_ERROR