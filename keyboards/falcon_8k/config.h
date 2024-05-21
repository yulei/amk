/**
 * config.h
 *
 */
#pragma once

#define VENDOR_ID       0x4D58      // MX
#define PRODUCT_ID      0xFAC8      // Falcon 8K
#define DEVICE_VER      0x0001
#define MANUFACTURER    Matrix Lab
#define PRODUCT         Falcon

#define MATRIX_ROWS     5
#define MATRIX_COLS     16

#define MATRIX_ROW_PINS     {B1, B0, A3, A2, A0};
#define MATRIX_COL_PINS     {C6, C7, C8, C9, C5, C4, C0, C1, C2, C3, C11, C10, C13, C12, C14, C15};
#define ROW_MASK {1<<6, 1<<7, 1<<8, 1<<9, 1<<5, 1<<4, 1<<0, 1<<1, 1<<2, 1<<3, 1<<11, 1<<10, 1<<13, 1<<12, 1<<14, 1<<15};

#define CAPS_LED_PIN        A1

//#define USE_I2C1
//#define I2C1_PINS           (GPIO_PIN_8 | GPIO_PIN_9)

#define USE_WS2812
#define USE_PWM_TIM4
#define PWM_TIM_PERIOD      104
#define PWM_TIM             htim4
#define PWM_TIM_CHANNEL     TIM_CHANNEL_2
#define PWM_TIM_DMA_ID      TIM_DMA_ID_CC2
#define PWM_DMA_INSTANCE    DMA1_Stream3
#define PWM_DMA_CHANNEL     DMA_CHANNEL_2
#define PWM_DMA_IRQHnadler  DMA1_Stream3_IRQHandler
#define PWM_DMA_IRQ         DMA1_Stream3_IRQn
#define PWM_PIN             GPIO_PIN_7

//#define WS2812_USE_GPIO
#define WS2812_LED_PIN      B7

#define WS2812_LED_NUM      4

// rgb configuration
#define RGB_LED_NUM             (WS2812_LED_NUM)
#define RGB_DEVICE_NUM          1
#define RGB_SEGMENT_NUM         1

#define VECT_TAB_OFFSET     0x20000
#define HSE_VALUE           16000000U

#define SUSPEND_RESET
#define DWT_DELAY
#define USE_HS_USB
//#define RGB_FLUSH_ASYNC

#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

#define VIAL_KEYBOARD_UID {0xBA, 0xA7, 0xF6, 0x19, 0xB4, 0xB2, 0x69, 0x2E}
