/**
 * config.h
 *
 */
#pragma once

#define VENDOR_ID       0x4543      // EC
#define PRODUCT_ID      0x494D      // IM
#define DEVICE_VER      0x0001
#define MANUFACTURER    Matrix Lab 
#define PRODUCT         IMP EC

#define MATRIX_ROWS     6
#define MATRIX_COLS     16
#define L_MASK          0x08
#define R_MASK          0x10

#define MATRIX_ROW_PINS {ROW_6_PIN, ROW_1_PIN, ROW_2_PIN, ROW_3_PIN, ROW_4_PIN, ROW_5_PIN}
#define MATRIX_COL_PINS {L_MASK|3, L_MASK|0, L_MASK|1, L_MASK|2, L_MASK|4, L_MASK|6, L_MASK|7, L_MASK|5, R_MASK|3, R_MASK|0, R_MASK|1, R_MASK|2, R_MASK|4, R_MASK|6, R_MASK|7, R_MASK|5}

#define DISCHARGE_WAIT_PRE  25
#define DISCHARGE_WAIT_POST 25
#define STROKE_PRESS        15
#define STROKE_RELEASE      8

#define DIRECT_KEY_PIN  A10
#define DIRECT_KEY_ROW  5
#define DIRECT_KEY_COL  10

#define ROW_6_PIN       B6
#define ROW_1_PIN       B5
#define ROW_2_PIN       B4
#define ROW_3_PIN       A15
#define ROW_4_PIN       B15
#define ROW_5_PIN       B1

#define LEFT_EN_PIN     B10
#define RIGHT_EN_PIN    B2
#define COL_A_PIN       B14
#define COL_B_PIN       B13
#define COL_C_PIN       B12

#define OPA_EN_PIN      A5
#define DISCHARGE_PIN   A6

#define USE_ADC1
#define ADC_INST        hadc1
#define KEY_IN_CHANNEL  ADC_CHANNEL_2
#define KEY_IN_PIN      GPIO_PIN_2
#define KEY_IN_PORT     GPIOA
#define EC_TH_LOW       1024
#define EC_TH_HIGH      1192
#define SENSE_TH        EC_TH_HIGH

#define CAPS_LED_PIN    A8
#define NUM_LED_PIN     B3

#define WS2812_LED_NUM  16
#define WS2812_LED_PIN  B7

#define USE_PWM_TIM4
#define PWM_TIM_PERIOD      119
#define PWM_PIN             GPIO_PIN_7
#define PWM_TIM             htim4
#define PWM_TIM_CHANNEL     TIM_CHANNEL_2
#define PWM_TIM_DMA_ID      TIM_DMA_ID_CC2
#define PWM_DMA_INSTANCE    DMA1_Stream3
#define PWM_DMA_CHANNEL     DMA_CHANNEL_2
#define PWM_DMA_IRQHnadler  DMA1_Stream3_IRQHandler

#define RGB_LED_NUM     (WS2812_LED_NUM+13)
#define RGB_DEVICE_NUM  2
#define RGB_SEGMENT_NUM 2

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE        10

#define VECT_TAB_OFFSET 0x20000
#define HSE_VALUE       16000000

#define USE_I2C1
#define IS31FL3731_I2C_ID   I2C_INSTANCE_1

#define DWT_DELAY
//#define WS2812_USE_GPIO

#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

#define VIAL_KEYBOARD_UID {0x61, 0xCF, 0xD5, 0xB6, 0x3B, 0x89, 0x09, 0xE9}
