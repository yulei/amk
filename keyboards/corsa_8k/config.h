/**
 * config.h
 *
 */
#pragma once

#define VENDOR_ID       0x4D58      // MX
#define PRODUCT_ID      0x3658      // M365 8K
#define DEVICE_VER      0x0001
#define MANUFACTURER    Matrix Lab
#define PRODUCT         Corsa

#define MATRIX_ROWS     5
#define MATRIX_COLS     16

#define MATRIX_ROW_PINS {A3, B0, A0, A1, A2};
#define MATRIX_COL_PINS {C3, C5, C4, C2, C15, C14, C13, C10, C11, C12, C1, C9, C6, C8, C7, C0};
#define ROW_MASK        {1<<3, 1<<5, 1<<4, 1<<2, 1<<15, 1<<14, 1<<13, 1<<10, 1<<11, 1<<12, 1<<1, 1<<9, 1<<6, 1<<8, 1<<7, 1<<0}

#define USE_I2C1
#define I2C1_PINS           GPIO_PIN_8 | GPIO_PIN_9

#define USE_SPI1
#define FLASH_CS            A4
#define W25QXX_SPI_ID       SPI_INSTANCE_1

// screen configuration
#define USE_SPI3
#define ST7735_SPI_ID       SPI_INSTANCE_3
#define SCREEN_NUM          1
#define SCREEN_0_PWR        A15
#define SCREEN_0_PWR_EN     0
#define SCREEN_0_CS         D2
#define SCREEN_0_RESET      B7
#define SCREEN_0_DC         B6
#define GC9107_AS_ST7735
#define SCREEN_ROTATION     3

// rgb configuration
#define USE_3729
#define IS31FL3729_SCALE_DEFAULT    0x7F
#define IS31FL3729_NUM              2
#define RGB_MATRIX_LED_NUM          75
#define RGB_MATRIX_LED_COUNT        RGB_MATRIX_LED_NUM

#define RGB_LED_NUM             (RGB_MATRIX_LED_NUM)
#define RGB_DEVICE_NUM          2
#define RGB_MATRIX_NUM          1

#define VECT_TAB_OFFSET     0x20000
#define HSE_VALUE           16000000U

#define SUSPEND_RESET
#define DWT_DELAY
#define USE_HS_USB
//#define RGB_FLUSH_ASYNC
#define VIAL_KEYBOARD_UID {0xDC, 0xA4, 0x37, 0x9A, 0xE9, 0x84, 0x97, 0xEA}

#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"
