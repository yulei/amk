/**
 * config.h
 *
 */

#pragma once
#define VENDOR_ID       0x4543      // EC
#define PRODUCT_ID      0x0365   // M365
#define DEVICE_VER      0x0001
#define MANUFACTURER    Matrix Lab
#define PRODUCT         Corsa EC

// key matrix
#define L_MASK          0x08
#define R_MASK          0x10
#define MATRIX_ROWS     5
#define MATRIX_COLS     14

#define MATRIX_ROW_PINS     {A15, B3, B2, B1, B0}
#define MATRIX_COL_PINS     {L_MASK|3, L_MASK|0, L_MASK|1, L_MASK|2, L_MASK|4, L_MASK|6, L_MASK|5, \
                            R_MASK|0, R_MASK|1, R_MASK|2, R_MASK|4, R_MASK|6, R_MASK|7, R_MASK|5}

#define DISCHARGE_WAIT_PRE  20
#define DISCHARGE_WAIT_POST 20

#define LEFT_EN_PIN     B5
#define RIGHT_EN_PIN    B9
#define COL_A_PIN       B8
#define COL_B_PIN       B7
#define COL_C_PIN       B6

#define DISCHARGE_PIN   A0

#define USE_ADC1
#define ADC_INST        hadc1
#define KEY_IN_CHANNEL  ADC_CHANNEL_2
#define KEY_IN_PIN      GPIO_PIN_2
#define KEY_IN_PORT     GPIOA
#define SENSE_TH        1024

#define CAPS_LED_PIN        A10

#define DEBOUNCE            8
#define DWT_DELAY
#define VECT_TAB_OFFSET     0x20000
#define HSE_VALUE           16000000U
#define SUSPEND_RESET
//#define FAULT_BREAK

#define USE_SPI1
#define FLASH_CS            A4
#define W25QXX_SPI_ID       SPI_INSTANCE_1

// screen configuration
#define USE_SPI2
#define ST7735_SPI_ID       SPI_INSTANCE_2
#define SCREEN_NUM          1
#define SCREEN_0_PWR        B10
#define SCREEN_0_PWR_EN     0
#define SCREEN_0_CS         B12
#define SCREEN_0_RESET      A9
#define SCREEN_0_DC         A8
#define GC9107_AS_ST7735
#define SCREEN_ROTATION     3


// vial 
#define VIAL_KEYBOARD_UID {0xE7, 0x09, 0xBA, 0x4A, 0x54, 0x05, 0xF8, 0x76}
#define DYNAMIC_KEYMAP_LAYER_COUNT  6

// for apc
#define APC_KEY_MIN             20
#define APC_KEY_MAX             2560

#define APC_INTERVAL_MIN        100
#define APC_INTERVAL_MAX        2560

#define APC_INTERVAL_INVALID    0
#define APC_INTERVAL_DEFAULT    1024
#define APC_INTERVAL_INDEX      8

#define DYNAMIC_KEYMAP_LAYER_COUNT  6
#define APC_KEYMAP_DOWN_LAYER       4
#define APC_KEYMAP_UP_LAYER         5
