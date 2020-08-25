/**
 * config.h
 *
 */

#pragma once

/* USB Device descriptor parameter */
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x20BE   // 8XV2.0 Additional BLE
#define DEVICE_VER 0x0001
#define MANUFACTURER MATRIX

/* key matrix */
#define MATRIX_ROWS 6
#define MATRIX_COLS 16

#define MATRIX_PIN uint16_t
#define DEF_PIN(port, pin) (((port)<<8)|(pin))

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

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE        5

#define GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))

#define MATRIX_DETECT_PIN           GPIO_PIN_MAP(0,10)

#define BATTERY_SAADC_ENABLE_PIN    GPIO_PIN_MAP(1,10)
#define BATTERY_SAADC_PIN           NRF_SAADC_INPUT_AIN3

#define I2C_INSTANCE_ID             0
#define I2C_SCL_PIN                 GPIO_PIN_MAP(0,13)
#define I2C_SDA_PIN                 GPIO_PIN_MAP(0,24)

#define INDICATOR_PIN               GPIO_PIN_MAP(0,30)