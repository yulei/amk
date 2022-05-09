/**
 * config.h
 *
 */
#pragma once

#define VENDOR_ID       0x4543      // EC
#define PRODUCT_ID      0x4D45      // ME
#define DEVICE_VER      0x0001
#define MANUFACTURER    astro
#define PRODUCT         ME EC

#define MATRIX_ROWS     6
#define MATRIX_COLS     16
#define L_MASK          0x08
#define R_MASK          0x10
#define MATRIX_ROW_PINS {ROW_6_PIN, ROW_1_PIN, ROW_2_PIN, ROW_3_PIN, ROW_4_PIN, ROW_5_PIN}
#define MATRIX_COL_PINS {L_MASK|3, L_MASK|0, L_MASK|1, L_MASK|2, L_MASK|4, L_MASK|6, L_MASK|7, L_MASK|5, R_MASK|3, R_MASK|0, R_MASK|1, R_MASK|2, R_MASK|4, R_MASK|6, R_MASK|7, R_MASK|5}

#define DISCHARGE_WAIT_PRE  30
#define DISCHARGE_WAIT_POST 30
#define CHARGE_WAIT     2

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
#define SENSE_TH        1024

#define CAPS_LED_PIN    A8
#define WS2812_LED_NUM  4
#define WS2812_LED_PIN  B7

#define USE_PWM_TIM4
#define PWM_TIM_PERIOD      119
#define PWM_TIM             htim4
#define PWM_TIM_CHANNEL     TIM_CHANNEL_2

#define RGB_LED_NUM     (4+12)
#define RGB_DEVICE_NUM  2
#define RGB_SEGMENT_NUM 2

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE        0

#define VECT_TAB_OFFSET 0x20000
#define USE_I2C1
#define DWT_DELAY

//#define WS2812_USE_GPIO