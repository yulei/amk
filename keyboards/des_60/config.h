/**
 * config.h
 *
 */
#pragma once

#define VENDOR_ID       0x4543      // EC
#define PRODUCT_ID      0x4453      // DS
#define DEVICE_VER      0x0001
#define MANUFACTURER    DESKEYS
#define PRODUCT         NEVEREST 60

#define MATRIX_ROWS     4
#define MATRIX_COLS     15
#define L_MASK          0x08
#define R_MASK          0x10

#define MATRIX_ROW_PINS {ROW_1_PIN, ROW_2_PIN, ROW_3_PIN, ROW_4_PIN}
#define MATRIX_COL_PINS {L_MASK|0, L_MASK|1, L_MASK|2, L_MASK|3, L_MASK|4, L_MASK|5, L_MASK|6, L_MASK|7, R_MASK|3, R_MASK|1, R_MASK|0, R_MASK|2, R_MASK|4, R_MASK|6, R_MASK|7};

#define DISCHARGE_WAIT_PRE  20
#define DISCHARGE_WAIT_POST 20
#define EC_TH_LOW       1680
#define EC_TH_HIGH      2368
#define SENSE_TH        EC_TH_HIGH

#define ROW_1_PIN       B15
#define ROW_2_PIN       A8
#define ROW_3_PIN       A9
#define ROW_4_PIN       B14

#define LEFT_EN_PIN     B6
#define RIGHT_EN_PIN    B7
#define COL_A_PIN       B3
#define COL_B_PIN       B4
#define COL_C_PIN       B5

#define OPA_EN_PIN      C13
#define DISCHARGE_PIN   C14

#define DIRECT_KEY_PIN  B12
//#define DIRECT_KEY_ROW  1
//#define DIRECT_KEY_COL  0

#define CAPS_LED_PIN    A15

#define USE_ADC1
#define ADC_INST        hadc1
#define KEY_IN_CHANNEL  ADC_CHANNEL_2
#define KEY_IN_PIN      GPIO_PIN_2
#define KEY_IN_PORT     GPIOA

//#define USE_PWM_TIM4
#define WS2812_LED_NUM  4
#define WS2812_LED_PIN  A10
#define PWM_TIM_PERIOD  119
#define PWM_TIM         htim4
#define PWM_TIM_CHANNEL TIM_CHANNEL_2
#define RGB_LED_NUM     4
#define RGB_DEVICE_NUM  1
#define RGB_SEGMENT_NUM 1

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE        5
#define DWT_DELAY

//#define VECT_TAB_OFFSET 0x20000
#define VECT_TAB_OFFSET 0x10000
#define HSE_VALUE       16000000U

#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

#define VIAL_INSECURE
#define VIAL_KEYBOARD_UID {0xA0, 0xC4, 0xD9, 0xA1, 0xE8, 0xAA, 0xFF, 0xE6}