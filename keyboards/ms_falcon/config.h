/**
 * config.h
 *
 */
#pragma once

#define VENDOR_ID       0x4D47      // MG
#define PRODUCT_ID      0x4643      // FC
#define DEVICE_VER      0x0001
#define MANUFACTURER    astro
#define PRODUCT         Falcon Mag

#define MATRIX_ROWS     4
#define MATRIX_COLS     15
#define L_MASK          0x08
#define R_MASK          0x10

#define MATRIX_ROW_PINS {B1, B0, A7, A6}
#define MATRIX_COL_PINS {L_MASK|0, L_MASK|1, L_MASK|2, L_MASK|3, L_MASK|4, L_MASK|5, L_MASK|6, L_MASK|7, \
                        R_MASK|4, R_MASK|7, R_MASK|6, R_MASK|5, R_MASK|3, R_MASK|0, R_MASK|1};

#define MAG_ZERO        2048
#define MAG_TH_LOW      2000
#define MAG_TH_HIGH     2300

#define LEFT_EN_PIN     B12 
#define RIGHT_EN_PIN    B13
#define COL_A_PIN       A8
#define COL_B_PIN       B15
#define COL_C_PIN       B14

#define USE_ADC1
#define ADC_INST        hadc1
#define KEY_IN_CHANNEL  ADC_CHANNEL_1
#define KEY_IN_PIN      GPIO_PIN_1
#define KEY_IN_PORT     GPIOA

#define CAPS_LED_PIN    A5

#define USE_PWM_TIM4
#define PWM_TIM_PERIOD      119
#define PWM_TIM             htim4
#define PWM_TIM_CHANNEL     TIM_CHANNEL_2
#define PWM_TIM_DMA_ID      TIM_DMA_ID_CC2

#define PWM_DMA_INSTANCE    DMA1_Stream3
#define PWM_DMA_CHANNEL     DMA_CHANNEL_2
#define PWM_DMA_IRQHnadler  DMA1_Stream3_IRQHandler
#define PWM_PIN             GPIO_PIN_7

#define USE_WS2812
#define WS2812_LED_NUM      4
#define WS2812_LED_PIN      B7

#define RGB_LED_NUM         WS2812_LED_NUM
#define RGB_DEVICE_NUM      1
#define RGB_SEGMENT_NUM     1

#define DEBOUNCE        1
#define DWT_DELAY
#define SUSPEND_RECONNECT

#define VECT_TAB_OFFSET 0x20000
#define HSE_VALUE       16000000

#define VIAL_KEYBOARD_UID {0xC4, 0xDD, 0x9C, 0xE7, 0xED, 0xE0, 0x71, 0xC8}

//#define APC_INTERVAL_MIN            300
#define DYNAMIC_KEYMAP_LAYER_COUNT  7
#define APC_KEYMAP_DOWN_LAYER       4
#define APC_KEYMAP_UP_LAYER         5
#define APC_KEYMAP_DKS_LAYER        6
