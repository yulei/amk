/**
 * config.h
 *
 */
#pragma once

#define VENDOR_ID       0x4D47      // MG
#define PRODUCT_ID      0x0365   // M365
#define DEVICE_VER      0x0001
#define MANUFACTURER    Matrix Lab
#define PRODUCT         Corsa MS

#define MATRIX_ROWS     5
#define MATRIX_COLS     14

#define MATRIX_COL_PINS {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
#define ADC_CHANNEL_COUNT   5
#define ADC_CHANNELS    {ADC_CHANNEL_4, ADC_CHANNEL_3, ADC_CHANNEL_2, ADC_CHANNEL_1, ADC_CHANNEL_0}
#define ADC_PINS        {A4, A3, A2, A1, A0}

#define MAG_ZERO        2048
#define MAG_TH_LOW      2200
#define MAG_TH_HIGH     2600

#define MS_CORSA_V2
#ifdef MS_CORSA_V2
#define SELECT_EN_PIN   B7
#define COL_A_PIN       C4
#define COL_B_PIN       C5
#define COL_C_PIN       B1
#define COL_D_PIN       B0

#define USE_ADC1
#define ADC_INST        hadc1
#define KEY_IN_CHANNEL  ADC_CHANNEL_1
#define KEY_IN_PIN      GPIO_PIN_1
#define KEY_IN_PORT     GPIOA

#define USE_I2C1
#define I2C1_PINS       GPIO_PIN_8 | GPIO_PIN_9

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

#else
#define SELECT_EN_PIN   A15
#define COL_A_PIN       B3
#define COL_B_PIN       B4
#define COL_C_PIN       B5
#define COL_D_PIN       B6

#define USE_ADC1
#define ADC_INST        hadc1
#define KEY_IN_CHANNEL  ADC_CHANNEL_1
#define KEY_IN_PIN      GPIO_PIN_1
#define KEY_IN_PORT     GPIOA

#define USE_I2C1
#define I2C1_SCL_PIN        B8
#define I2C1_SDA_PIN        B9

#define USE_SPI1
#define FLASH_CS            B0
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
#endif

#define USE_3729
#define IS31FL3729_SCALE_DEFAULT    0x7F
#define IS31FL3729_NUM          2
#define RGB_MATRIX_LED_NUM      68
#define RGB_MATRIX_LED_COUNT    RGB_MATRIX_LED_NUM

#define RGB_LED_NUM             (RGB_MATRIX_LED_NUM)
#define RGB_DEVICE_NUM          2
#define RGB_MATRIX_NUM          1

#define GC9107_AS_ST7735
#define SCREEN_ROTATION     3

#define DEBOUNCE            0
#define SUSPEND_RESET
#define DWT_DELAY
#define VECT_TAB_OFFSET     0x20000
#define HSE_VALUE           16000000U

#define VIAL_KEYBOARD_UID {0xA6, 0xE2, 0x4B, 0x4D, 0x7E, 0x09, 0xB3, 0x12}
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

#define USE_HS_USB
//#define RGB_FLUSH_ASYNC


//#define APC_SPECIAL_KEY_NUM     1
#ifdef APC_SPECIAL_KEY_NUM
#define APC_INTERVAL_PRESET 1000
#define APC_KEY_MIN_PRESET  1700
#define APC_KEY_MAX_PRESET  2700

#define APC_KEY_MIN             1500
#define APC_KEY_MAX             3276

#define AMK_RT_SENS_DEFAULT     20 
#define AMK_TOP_SENS_DEFAULT    20
#define AMK_BTM_SENS_DEFAULT    20
#define AMK_APC_SENS_DEFAULT    20
#define AMK_APC_NOISE_DEFAULT   20
#endif