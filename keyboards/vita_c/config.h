/**
 * config.h
 *
 */

#pragma once

// USB Device descriptor parameter
#define VENDOR_ID       0x4D58   // MX
#define PRODUCT_ID      0x6505   // M65 with 1u screen
#define DEVICE_VER      0x0001
#define MANUFACTURER    Matrix Lab
#define PRODUCT         Vita Classic

// key matrix
#define MATRIX_ROWS     5
#define MATRIX_COLS     15

#define MATRIX_ROW_PINS { A1, A15, D2, C12, B1}
#define MATRIX_COL_PINS { A8, A10, C9, C8, C7, C6, B7, B6, B5, B4, C10, B10, B11, B2, B0}

#define USE_SPI1
#define SCREEN_ROTATION     1
#define SCREEN_DRIVER_NUM   1
#define SCREEN_NUM          1
#define DISPLAY_NUM         1
#define ANIM_DISPLAY_NUM    1

#define SCREEN_NUM          1
#define SCREEN_0_PWR        C0
#define SCREEN_0_PWR_EN     1
#define SCREEN_0_CS         A2
#define SCREEN_0_RESET      A3
#define SCREEN_0_DC         A4
#define SSD1357_SPI_ID      SPI_INSTANCE_1

#define USE_SPI2
#define FLASH_CS            B12
#define W25QXX_SPI_ID       SPI_INSTANCE_2

//#define USE_I2C1
#define I2C1_SCL_PIN            B8
#define I2C1_SDA_PIN            B9

#define USE_WS2812
#define WS2812_LED_NUM          5
#define WS2812_LED_PIN          A0
#define PWM_TIM_PERIOD          104
#define PWM_TIM                 htim2
#define PWM_TIM_CHANNEL         TIM_CHANNEL_1

#define RGB_LED_NUM             5
#define RGB_DEVICE_NUM          1
#define RGB_SEGMENT_NUM         2

#define VIAL_KEYBOARD_UID {0xD6, 0x04, 0xE0, 0x1E, 0x8E, 0xC0, 0xA1, 0xE4}

//#define WDT_ENABLE
//#define FAULT_BREAK
//#define TYPING_SPEED

#define SUSPEND_RECONNECT
#define DWT_DELAY
#define VECT_TAB_OFFSET 0x20000

#define FLASH_BASE_ADDRESS      0x8060000
#define FLASH_TOTAL_SIZE        0x20000
#define FLASH_SECTOR_ID         FLASH_SECTOR_7
