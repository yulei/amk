/**
 * config.h
 *
 */

#pragma once

#define VENDOR_ID       0x4D58   // MX
#define PRODUCT_ID      0x0802   // 8xv2
#define DEVICE_VER      0x0001
#define MANUFACTURER    Matrix Lab
#define PRODUCT         8XV2 OG 

// key matrix
#define MATRIX_ROWS         6
#define MATRIX_COLS         16
#define MATRIX_ROW_PINS     {A15, B5, B6, A10, A7, B3}
#define MATRIX_COL_PINS     {C9, C8, C7, C6, C5, C4, C3, C2, C1, C0, C15, C14, C13, C12, C11, C10};

#define CAPS_LED_PIN        A8
#define SCROLL_LED_PIN      B4
//#define USE_WS2812
#define USE_3731

#define USE_I2C1
//#define USE_I2C_SW
#define I2C_SW_SCL_PIN      B9
#define I2C_SW_SDA_PIN      B8
//#define IS31FL3731_I2C_ID   I2C_INSTANCE_1

//#define WS2812_LED_NUM      7
//#define WS2812_LED_PIN      B6

//#define USE_PWM_TIM4
//#define PWM_TIM_PERIOD      119
//#define PWM_TIM             htim4
//#define PWM_TIM_CHANNEL     TIM_CHANNEL_1
//#define PWM_TIM_DMA_ID      TIM_DMA_ID_CC1
//#define PWM_DMA_INSTANCE    DMA1_Stream0
//#define PWM_DMA_CHANNEL     DMA_CHANNEL_2
//#define PWM_DMA_IRQHnadler  DMA1_Stream0_IRQHandler
//#define PWM_PIN             GPIO_PIN_6

#define RGB_LED_NUM         20 //(WS2812_LED_NUM+RGB_MATRIX_LED_NUM)
#define RGB_DEVICE_NUM      1
#define RGB_SEGMENT_NUM     2


#define DEBOUNCE            5
#define DWT_DELAY
#define HSE_VALUE           16000000
#define VECT_TAB_OFFSET     0x20000
//#define FAULT_BREAK
#define SUSPEND_RECONNECT

// vial 
#define VIAL_KEYBOARD_UID {0x93, 0x77, 0x6D, 0x8A, 0xC0, 0x95, 0x52, 0xDD}

// custom tusb definitions
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"
