/**
 * config.h
 *
 */

#pragma once

/* USB Device descriptor parameter */
#define VENDOR_ID       0x4D58   // MX
#define PRODUCT_ID      0x20BE   // 8XV2.0 Additional BLE
#define DEVICE_VER      0x0001
#define MANUFACTURER    ASTRO
#define PRODUCT         8XV2.0 Additional
#define BLE_PRODUCT     8XV2.0 BLE

/* key matrix */
#define MATRIX_ROWS 6
#define MATRIX_COLS 16

#define DEF_PIN(port, pin) (((port) << 8) | pin)
#define GET_PORT(pp) (((pp) >> 8) & 0xFF)
#define GET_PIN(pp) ((pp) & 0xFF)

#define MATRIX_ROW_PINS { \
    DEF_PIN(TCA6424_PORT2, 7), \
    DEF_PIN(TCA6424_PORT2, 6), \
    DEF_PIN(TCA6424_PORT2, 0), \
    DEF_PIN(TCA6424_PORT2, 2), \
    DEF_PIN(TCA6424_PORT2, 4), \
    DEF_PIN(TCA6424_PORT2, 5) }

#define MATRIX_COL_PINS { \
    DEF_PIN(TCA6424_PORT2, 1), \
    DEF_PIN(TCA6424_PORT1, 7), \
    DEF_PIN(TCA6424_PORT1, 6), \
    DEF_PIN(TCA6424_PORT1, 5), \
    DEF_PIN(TCA6424_PORT1, 4), \
    DEF_PIN(TCA6424_PORT1, 3), \
    DEF_PIN(TCA6424_PORT1, 2), \
    DEF_PIN(TCA6424_PORT1, 1), \
    DEF_PIN(TCA6424_PORT1, 0), \
    DEF_PIN(TCA6424_PORT0, 7), \
    DEF_PIN(TCA6424_PORT0, 6), \
    DEF_PIN(TCA6424_PORT0, 5), \
    DEF_PIN(TCA6424_PORT0, 4), \
    DEF_PIN(TCA6424_PORT0, 3), \
    DEF_PIN(TCA6424_PORT0, 2), \
    DEF_PIN(TCA6424_PORT0, 1) }


#define ROW1_MASK 0x80
#define ROW2_MASK 0x40
#define ROW3_MASK 0x01
#define ROW4_MASK 0x04
#define ROW5_MASK 0x10
#define ROW6_MASK 0x20
#define ROW_PORT TCA6424_PORT2

#define COL1_MASK 0x02
#define COL2_MASK 0x80
#define COL3_MASK 0x40
#define COL4_MASK 0x20
#define COL5_MASK 0x10
#define COL6_MASK 0x08
#define COL7_MASK 0x04
#define COL8_MASK 0x02
#define COL9_MASK 0x01
#define COL10_MASK 0x80
#define COL11_MASK 0x40
#define COL12_MASK 0x20
#define COL13_MASK 0x10
#define COL14_MASK 0x08
#define COL15_MASK 0x04
#define COL16_MASK 0x02

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE        5

#define GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))

#define MATRIX_INT_PIN              GPIO_PIN_MAP(0,3)

#define BATTERY_SAADC_ENABLE_PIN    GPIO_PIN_MAP(0,31)
#define BATTERY_SAADC_PIN           NRF_SAADC_INPUT_AIN6

#define I2C_USE_INSTANCE_1
#define I2C1_SCL_PIN                GPIO_PIN_MAP(0,5)
#define I2C1_SDA_PIN                GPIO_PIN_MAP(1,9)

#define WS2812_LED_NUM              4
#define WS2812_LED_PIN              2
#define RGB_LED_NUM                 20
#define RGB_DEVICE_NUM              1
#define RGB_SEGMENT_NUM             2

#define WDT_ENABLE                  1        