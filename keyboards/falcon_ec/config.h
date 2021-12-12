/**
 * config.h
 *
 */
#pragma once

#define VENDOR_ID       0x4543      // EC
#define PRODUCT_ID      0x4643      // FC
#define DEVICE_VER      0x0001
#define MANUFACTURER    astro
#define PRODUCT         Falcon EC

#define MATRIX_ROWS     4
#define MATRIX_COLS     16
#define MATRIX_ROW_PINS {}
#define MATRIX_COL_PINS {}

#define ROW_1_PIN       B13
#define ROW_2_PIN       B14
#define ROW_3_PIN       B15
#define ROW_4_PIN       B12

#define LEFT_EN_PIN     A7
#define RIGHT_EN_PIN    A8
#define COL_A_PIN       B2
#define COL_B_PIN       B1
#define COL_C_PIN       B0

#define OPA_EN_PIN      A4
#define DISCHARGE_PIN   C13

#define KEY_IN_CHANNEL  ADC_CHANNEL_2
#define KEY_IN_PIN      GPIO_PIN_2
#define KEY_IN_PORT     GPIOA
#define SENSE_TH        1284

#define CAPS_LED_PIN    A6

#define WS2812_LED_NUM  4
#define WS2812_LED_PIN  B7
#define PWM_TIM_PERIOD  119
#define PWM_TIM         htim4
#define PWM_TIM_CHANNEL TIM_CHANNEL_2
#define RGB_LED_NUM     4
#define RGB_DEVICE_NUM  1
#define RGB_SEGMENT_NUM 1

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
//#define DEBOUNCE        5

#define VECT_TAB_OFFSET 0x20000
#define USE_ADC1

#define HSE_VALUE    16000000U
