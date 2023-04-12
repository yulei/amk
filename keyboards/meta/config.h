/**
 * config.h
 *
 */

#pragma once

#define VENDOR_ID           0x4D58   // MX
#define PRODUCT_ID          0x4D54   // MT
#define DEVICE_VER          0x0002
#define MANUFACTURER        Matrix Lab
#define PRODUCT             Meta 

// key matrix
#define MATRIX_ROWS         6
#define MATRIX_COLS         16
#define MATRIX_ROW_PINS     {C8,C7,C6,B15,B14,A2}
#define MATRIX_COL_PINS     {A4,A15,A5,A6,A7,C4,C5,B0,B1,B2,B10,B12,B13,C13,C14,C15}
#define USB_EN_PIN          A1
#define SDB_EN_PIN          C11

#define USE_UART1

#define META_HOTSWAP
#if defined(META_HOTSWAP)
#define USE_PWM_TIM2
#define PWM_TIM             htim2
#define PWM_TIM_PERIOD      104
#define PWM_TIM_CHANNEL     TIM_CHANNEL_4
#define PWM_TIM_DMA_ID      TIM_DMA_ID_CC4

#define PWM_DMA_TIM_CH      hdma_tim2_ch4     
#define PWM_DMA_INSTANCE    DMA1_Stream6
#define PWM_DMA_CHANNEL     DMA_CHANNEL_3 
#define PWM_DMA_IRQHnadler  DMA1_Stream6_IRQHandler

#define WS2812_LED_NUM      17
#define WS2812_LED_PIN      A3
#define RGB_LED_NUM         (WS2812_LED_NUM)
#define RGB_DEVICE_NUM      1
#define RGB_INDICATOR_LED_NUM    1
#define RGB_SEGMENT_NUM     1
#else

#define USE_I2C1
#define IS31FL3729_SCALE_DEFAULT    0x4F
#define IS31FL3729_I2C_ID   I2C_INSTANCE_1
#define IS31FL3731_I2C_ID   I2C_INSTANCE_1
#define IS31FL3729_NUM      2
#define RGB_MATRIX_LED_NUM  90
#define RGB_LED_NUM         (RGB_MATRIX_LED_NUM+0)

#define RGB_DEVICE_NUM      2
#define RGB_SEGMENT_NUM     1
#define RGB_MATRIX_NUM      1

#define CAPS_LED_PIN        A3
#endif


#define HSE_VALUE           12000000
#define VECT_TAB_OFFSET     0x20000

#define DEBOUNCE            5

// vial 
#define VIAL_KEYBOARD_UID {0xD6, 0xED, 0x12, 0x90, 0x54, 0x52, 0x34, 0xFF}

// custom tusb definitions
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

#define SUSPEND_RECONNECT
