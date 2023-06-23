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
#define MATRIX_COLS     15
#define L_MASK          0x08
#define R_MASK          0x10

#define ROW_1_PIN       B13
#define ROW_2_PIN       B14
#define ROW_3_PIN       B15
#define ROW_4_PIN       B12

#define MATRIX_ROW_PINS {ROW_1_PIN, ROW_2_PIN, ROW_3_PIN, ROW_4_PIN}
#define MATRIX_COL_PINS {L_MASK|3, L_MASK|0, L_MASK|1, L_MASK|2, L_MASK|4, L_MASK|6, L_MASK|7, L_MASK|5, R_MASK|3, R_MASK|0, R_MASK|1, R_MASK|2, R_MASK|4, R_MASK|6, R_MASK|7};

#define DISCHARGE_WAIT_PRE  30
#define DISCHARGE_WAIT_POST 30
#define CHARGE_WAIT     2
#define EC_TH_LOW       600
#define EC_TH_HIGH      800

#define LEFT_EN_PIN     A7
#define RIGHT_EN_PIN    A8
#define COL_A_PIN       B2
#define COL_B_PIN       B1
#define COL_C_PIN       B0

#define OPA_EN_PIN      A4
#define DISCHARGE_PIN   C13

#define USE_ADC1
#define ADC_INST        hadc1
#define KEY_IN_CHANNEL  ADC_CHANNEL_2
#define KEY_IN_PIN      GPIO_PIN_2
#define KEY_IN_PORT     GPIOA
#define SENSE_TH        1192//EC_TH_HIGH

#define CAPS_LED_PIN    A6

#define WS2812_LED_NUM  4
#define WS2812_LED_PIN  B7

#define USE_PWM_TIM4
#define PWM_TIM_PERIOD      119
#define PWM_TIM             htim4
#define PWM_TIM_CHANNEL     TIM_CHANNEL_2
#define PWM_TIM_DMA_ID      TIM_DMA_ID_CC2

#define PWM_DMA_INSTANCE    DMA1_Stream3
#define PWM_DMA_CHANNEL     DMA_CHANNEL_2
#define PWM_DMA_IRQHnadler  DMA1_Stream3_IRQHandler
#define PWM_PIN             GPIO_PIN_7

#define RGB_LED_NUM     4
#define RGB_DEVICE_NUM  1
#define RGB_SEGMENT_NUM 1

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE        10
#define DWT_DELAY
#define SUSPEND_RECONNECT

#define VECT_TAB_OFFSET 0x20000
#define HSE_VALUE       16000000

#define VIAL_KEYBOARD_UID {0x7D, 0xCE, 0x7A, 0x3A, 0xE0, 0xE5, 0xBC, 0xF0}
