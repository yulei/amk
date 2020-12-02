/**
 * config.h
 *
 */
#pragma once

/* USB Device descriptor parameter */
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0xBE60   // hhkb ble
#define DEVICE_VER 0x0001
#define MANUFACTURER astro
#define PRODUCT Matrix Falcon

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 14
#define MATRIX_ROW_PINS { 2, 31, 30,  3, 29}
#define MATRIX_COL_PINS { 9,  8,  7,  6,  5,  4, 10, 24, 23, 17, 16, 15, 14, 13}
/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE                    5

#define VBUS_DETECT_PIN             26
#define UART_RX_PIN                 19
#define UART_TX_PIN                 20
#define BATTERY_SAADC_ENABLE_PIN    27
#define BATTERY_SAADC_PIN           NRF_SAADC_INPUT_AIN4

#define I2C_INSTANCE_ID             0
#define I2C_SCL_PIN                 11
#define I2C_SDA_PIN                 12

#define RGBLIGHT_EN_PIN             22
#define RGBLIGHT_EN_HIGH

#define RGB_LED_NUM                 4
//#define WS2812_LED_PIN              18

#define AW9523B_ADDR                0xB6

#define CAPS_LED_PIN                18

#define INDICATOR_PIN               25

//#define GZLL_IS_CLIENT              1

#define WS2812_LED_PIN              12
#define DISABLE_USB
#define SLEEP_COUNT_MAX             2