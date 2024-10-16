/**
 * config.h
 *
 */

#pragma once

#define VENDOR_ID       0x4D58   // MX
#define PRODUCT_ID      0x0365   // M365
#define DEVICE_VER      0x0001
#define MANUFACTURER    Matrix Lab
#define PRODUCT         Corsa

// key matrix
#define LEFT_MASK   0x08
#define RIGHT_MASK  0x10
#define MATRIX_ROWS         5
#define MATRIX_COLS         15

#define MATRIX_ROW_PINS     {B0, B2, A15, B5, B4}
#define MATRIX_COL_PINS     {LEFT_MASK|0, LEFT_MASK|1, LEFT_MASK|2, LEFT_MASK|3, LEFT_MASK|4, LEFT_MASK|5, LEFT_MASK|6, LEFT_MASK|7, \
                        RIGHT_MASK|0, RIGHT_MASK|1, RIGHT_MASK|2, RIGHT_MASK|3, RIGHT_MASK|4, RIGHT_MASK|5, RIGHT_MASK|6, RIGHT_MASK|7}

#define LEFT_EN_PIN         A3
#define RIGHT_EN_PIN        A1
#define COL_A_PIN           C13
#define COL_B_PIN           C14
#define COL_C_PIN           A0


#define USE_I2C1
#define I2C1_SCL_PIN        B8
#define I2C1_SDA_PIN        B9

//#define CAPS_LED_PIN        B7
#define USE_3729
#ifdef RGB_MATRIX_ENABLE
//#define IS31FL3729_I2C_ID   I2C_INSTANCE_1
#define IS31FL3729_SCALE_DEFAULT    0x4F
#define IS31FL3729_NUM          2
#define RGB_MATRIX_LED_NUM      68
#define RGB_MATRIX_LED_COUNT    RGB_MATRIX_LED_NUM

#define RGB_LED_NUM             (RGB_MATRIX_LED_NUM)
#define RGB_DEVICE_NUM          2
#define RGB_MATRIX_NUM          1
#else
#define USE_3236
#define RGB_DEVICE_NUM              1
#define RGB_LED_NUM                 2
#define RGB_SEGMENT_NUM             1
#endif

#define DEBOUNCE            5
#define DWT_DELAY
#define VECT_TAB_OFFSET     0x20000
#define HSE_VALUE           16000000U
#define FAULT_BREAK

#define USE_SPI1
#define FLASH_CS            A4
#define W25QXX_SPI_ID       SPI_INSTANCE_1

// screen configuration
#define USE_SPI2
#define ST7735_SPI_ID       SPI_INSTANCE_2
#define SCREEN_NUM          1
#define SCREEN_0_PWR        B1
#define SCREEN_0_PWR_EN     0
#define SCREEN_0_CS         B12
#define SCREEN_0_RESET      A9
#define SCREEN_0_DC         A8

#define GC9107_AS_ST7735

#define SCREEN_ROTATION     3
//#define SCREEN_DRIVER_NUM   1
//#define SCREEN_NUM          1
//#define DISPLAY_NUM         1
//#define ANIM_DISPLAY_NUM    1
//#define AUDIO_DISPLAY_NUM   1
//#define RENDER_BUFFER_SIZE  (60*60*2)

// vial 
//#define VIAL_INSECURE
#define VIAL_KEYBOARD_UID {0x70, 0xB9, 0xB7, 0xE3, 0xB6, 0x82, 0x82, 0xFC}

// custom tusb definitions
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

#define SUSPEND_RECONNECT
//#define RGB_FLUSH_ASYNC

#define FRAME_MAX   256
