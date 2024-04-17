/**
 * config.h
 *
 */

#pragma once

#define VENDOR_ID           0x4D47  // MG
#define PRODUCT_ID          0x4E56  // NV
#define DEVICE_VER          0x0001
#define MANUFACTURER        Matrix Lab
#define PRODUCT             Navi

// key matrix
#define MATRIX_ROWS     5
#define MATRIX_COLS     16
#define MATRIX_NO_KEY_MASK {0,0,1<<8,0,0x800F}

#define MATRIX_COL_PINS {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
#define ADC_CHANNEL_COUNT   5
#define ADC_CHANNELS    {ADC_CHANNEL_2, ADC_CHANNEL_4, ADC_CHANNEL_0, ADC_CHANNEL_3, ADC_CHANNEL_1}
#define ADC_PINS        {A2, A4, A0, A3, A1}

#define COL_A_PIN       B10
#define COL_B_PIN       B2
#define COL_C_PIN       B1
#define COL_D_PIN       B0
#define SELECT_PIN      B11

#define USE_ADC1
#define ADC_INST        hadc1
#define KEY_IN_CHANNEL  ADC_CHANNEL_1
#define KEY_IN_PIN      GPIO_PIN_1
#define KEY_IN_PORT     GPIOA

//#define USB_EN_PIN      B12 

#define CAPS_LED_PIN   C5

#define USE_3236
#define USE_I2C1
#define I2C1_PINS       GPIO_PIN_8 | GPIO_PIN_9
//#define RGB_INIT_DELAY              2000

#define RGB_DEVICE_NUM      1
#define RGB_LED_NUM         3
#define RGB_SEGMENT_NUM     1

//#define USE_UART1

//#define HSE_VALUE           12000000
#define HSE_VALUE           16000000
#define VECT_TAB_OFFSET     0x20000

// vial 
#define VIAL_KEYBOARD_UID {0x8E, 0x29, 0x94, 0xA9, 0xE0, 0x81, 0x96, 0xC5}

// custom tusb definitions
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

#define SUSPEND_RESET
#define DWT_DELAY
#define USE_HS_USB
//#define RGB_FLUSH_ASYNC

#define USE_SPI1
#define SPI1_PINS           GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
#define SPI1_PORT           GPIOB
#define FLASH_CS            D2
#define W25QXX_SPI_ID       SPI_INSTANCE_1

// screen configuration
#define USE_SPI3
#define SPI3_PINS           GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
#define SPI3_PORT           GPIOC
#define ST7735_SPI_ID       SPI_INSTANCE_3
#define SCREEN_NUM          1
#define SCREEN_0_PWR        A8
#define SCREEN_0_PWR_EN     0
#define SCREEN_0_CS         A15
#define SCREEN_0_RESET      A9
#define SCREEN_0_DC         A10

#define GC9107_AS_ST7735
#define SCREEN_ROTATION     3

#define LAYOUT_current      LAYOUT_all

#define APC_INTERVAL_PRESET 800
#define APC_KEY_MIN_PRESET  2150
#define APC_KEY_MAX_PRESET  3000 

#define AMK_RT_SENS_DEFAULT     20 
#define AMK_TOP_SENS_DEFAULT    20
#define AMK_BTM_SENS_DEFAULT    50
#define AMK_APC_SENS_DEFAULT    20
#define AMK_APC_NOISE_DEFAULT   20

#define APC_SPECIAL_KEY_NUM     3 
