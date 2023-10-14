/**
 * config.h
 *
 */
#pragma once

#define VENDOR_ID       0x4543      // EC
#define PRODUCT_ID      0x0830      // 8xv3
#define DEVICE_VER      0x0001
#define MANUFACTURER    Matrix
#define PRODUCT         8xv3 EC

#define MATRIX_ROWS     6
#define MATRIX_COLS     16
#define L_MASK          0x08
#define R_MASK          0x10
#define MATRIX_ROW_PINS {B2, B1, B10, C3, C2, C1}
#define MATRIX_COL_PINS {L_MASK|2, L_MASK|1, L_MASK|0, L_MASK|3, L_MASK|5, L_MASK|7, L_MASK|6, L_MASK|4, R_MASK|2, R_MASK|1, R_MASK|0, R_MASK|3, R_MASK|5, R_MASK|7, R_MASK|6, R_MASK|4}

#define DISCHARGE_WAIT_PRE  20
#define DISCHARGE_WAIT_POST 20
#define EC_TH_LOW       1024
#define EC_TH_HIGH      1280

#define LEFT_EN_PIN     C5
#define RIGHT_EN_PIN    B0
#define COL_A_PIN       C4
#define COL_B_PIN       A7
#define COL_C_PIN       A6

//#define OPA_EN_PIN      A5
#define DISCHARGE_PIN   A5

#define USE_ADC1
#define ADC_INST        hadc1
#define KEY_IN_CHANNEL  ADC_CHANNEL_1
#define KEY_IN_PIN      GPIO_PIN_1
#define KEY_IN_PORT     GPIOA
#define SENSE_TH        1024

#define USE_SPI3
#define FLASH_CS            A15
#define W25QXX_SPI_ID       SPI_INSTANCE_3

// screen configuration
#define USE_SPI2
#define ST7735_SPI_ID       SPI_INSTANCE_2
#define SCREEN_NUM          1
#define SCREEN_0_PWR        C8
#define SCREEN_0_PWR_EN     0
#define SCREEN_0_CS         B12
#define SCREEN_0_RESET      C7
#define SCREEN_0_DC         C6

#define SCREEN_ROTATION     1
#define SCREEN_DRIVER_NUM   1
#define SCREEN_NUM          1
#define DISPLAY_NUM         1
#define ANIM_DISPLAY_NUM    1
//#define AUDIO_DISPLAY_NUM   1
#define RENDER_BUFFER_SIZE  (60*60*2)

#define CAPS_LED_PIN    A8

#if 1
#define USE_WS2812
#define USE_3236

#define WS2812_LED_NUM      7
#define WS2812_LED_PIN      B7

#define USE_PWM_TIM4
#define PWM_TIM_PERIOD      224
#define PWM_TIM             htim4
#define PWM_TIM_CHANNEL     TIM_CHANNEL_2
#define PWM_TIM_DMA_ID      TIM_DMA_ID_CC2
#define PWM_DMA_INSTANCE    DMA1_Stream3
#define PWM_DMA_CHANNEL     DMA_CHANNEL_2
#define PWM_DMA_IRQHnadler  DMA1_Stream3_IRQHandler

#define USE_I2C1
#define RGB_LED_NUM         (WS2812_LED_NUM+6)
#define RGB_DEVICE_NUM      2
#define RGB_SEGMENT_NUM     3

#endif

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
//#define DEBOUNCE        10
#define HSE_VALUE       16000000
#define VECT_TAB_OFFSET 0x20000
#define DWT_DELAY
#define SUSPEND_RECONNECT

#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

#define VIAL_KEYBOARD_UID {0x46, 0x52, 0xB3, 0xFD, 0xD6, 0x01, 0xD1, 0x3B}

// for apc
#define APC_KEY_MIN             20
#define APC_KEY_MAX             2560

#define APC_INTERVAL_MIN        100
#define APC_INTERVAL_MAX        2560

#define APC_INTERVAL_INVALID    0
#define APC_INTERVAL_DEFAULT    1024
#define APC_INTERVAL_INDEX      8

#define DYNAMIC_KEYMAP_LAYER_COUNT  4
#define APC_KEYMAP_DOWN_LAYER   2
#define APC_KEYMAP_UP_LAYER     3
