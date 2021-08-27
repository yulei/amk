/**
 * config.h
 *
 */
#pragma once

/* USB Device descriptor parameter */
#define VENDOR_ID       0x4D58   // MX
#define PRODUCT_ID      0xBE65   // Vita ble
#define DEVICE_VER      0x0001
#define MANUFACTURER    astro
#define PRODUCT         Vita BLE

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 15
#define MATRIX_ROW_PINS { 28, 29, 30, 9, 13}
#define MATRIX_COL_PINS { 24, 23, 22, 7, 6, 4, 3, 10, 2, 1, 0, 31, 18, 17, 16}

#define BOOT_ROW_PIN    28
#define BOOT_COL_PIN    24

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE                    5

#define VBUS_DETECT_PIN             11
#define UART_RX_PIN                 20
#define UART_TX_PIN                 19
#define BATTERY_SAADC_ENABLE_PIN    8
#define BATTERY_SAADC_PIN           NRF_SAADC_INPUT_AIN3

#define I2C_USE_INSTANCE_1
#define I2C1_SCL_PIN                27
#define I2C1_SDA_PIN                26

#define RGB_LED_NUM                 6
#define RGBLIGHT_EN_PIN             14
#define RGBLIGHT_EN_HIGH

#define RGB_DEVICE_NUM              2
#define RGB_SEGMENT_NUM             1
#define WS2812_LED_NUM              4
#define WS2812_LED_PIN              15

#define RGB_INDICATOR_LED_NUM       2
#define INDICATOR_SHDN_PIN          25

#define WDT_ENABLE                  1        

//#define GZLL_IS_CLIENT              1
//#define DISABLE_USB
//#define SLEEP_COUNT_MAX             2
//#define RESET_ON_ERROR