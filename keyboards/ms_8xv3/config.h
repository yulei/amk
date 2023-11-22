/**
 * config.h
 *
 */
#pragma once

#define VENDOR_ID       0x4D47      // MG
#define PRODUCT_ID      0x3858      // 8X
#define DEVICE_VER      0x0003
#define MANUFACTURER    Matrix Lab
#define PRODUCT         8XV3.0 MS

#define MATRIX_ROWS     6
#define MATRIX_COLS     15

//#define MATRIX_ROW_PINS {A9, A10, C11, A8}
#define MATRIX_COL_PINS {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};

#define MAG_ZERO        2048
#define MAG_TH_LOW      2200
#define MAG_TH_HIGH     2600

#define COL_A_PIN       C4
#define COL_B_PIN       C5 
#define COL_C_PIN       B1
#define COL_D_PIN       B0
#define SELECT_PIN      B2

#define USE_ADC1
#define ADC_INST        hadc1
#define KEY_IN_CHANNEL  ADC_CHANNEL_1
#define KEY_IN_PIN      GPIO_PIN_1
#define KEY_IN_PORT     GPIOA

// screen
#define USE_SPI1
#define SPI1_PINS           GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
#define SPI1_PORT           GPIOB
#define FLASH_CS            D2
#define W25QXX_SPI_ID       SPI_INSTANCE_1

// screen configuration
#define USE_SPI3
#define SPI3_PINS           GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
#define SPI3_PORT           GPIOC
#define ST7735_SPI_ID       SPI_INSTANCE_3
#define SCREEN_NUM          1
#define SCREEN_0_PWR        A8
#define SCREEN_0_PWR_EN     0
#define SCREEN_0_CS         A15
#define SCREEN_0_RESET      A9
#define SCREEN_0_DC         A10

#define SCREEN_ROTATION     1
#define SCREEN_DRIVER_NUM   1
#define SCREEN_NUM          1
#define DISPLAY_NUM         1
#define ANIM_DISPLAY_NUM    1
#define RENDER_BUFFER_SIZE  (60*60*2)

// rgb led
#define RGB_DEVICE_NUM  4

#define USE_3729
#define IS31FL3729_SCALE_DEFAULT 0x6F
#define IS31FL3729_NUM      2
#define RGB_MATRIX_LED_NUM  90
#define RGB_MATRIX_LED_COUNT    RGB_MATRIX_LED_NUM
#define RGB_MATRIX_NUM      1

#define USE_3236
#define RGB_SEGMENT_NUM     3
#define LED_NUM_3236        6

#define USE_WS2812
#define WS2812_LED_NUM      7
#define WS2812_LED_PIN      B7

#define USE_PWM_TIM4
#define PWM_TIM_PERIOD      104
#define PWM_TIM             htim4
#define PWM_TIM_CHANNEL     TIM_CHANNEL_2
#define PWM_TIM_DMA_ID      TIM_DMA_ID_CC2
#define PWM_DMA_INSTANCE    DMA1_Stream3
#define PWM_DMA_CHANNEL     DMA_CHANNEL_2
#define PWM_DMA_IRQHnadler  DMA1_Stream3_IRQHandler

#define RGB_LED_NUM         (RGB_MATRIX_LED_NUM + LED_NUM_3236 + WS2812_LED_NUM)

#define USE_I2C1
#define I2C1_PINS           (GPIO_PIN_8|GPIO_PIN_9)

#define DWT_DELAY
#define SUSPEND_RESET

#define VECT_TAB_OFFSET 0x20000
#define HSE_VALUE       16000000

#define VIAL_KEYBOARD_UID {0xAC, 0x0D, 0xCB, 0x0E, 0xCC, 0xB1, 0x0E, 0x07}
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

#define APC_INTERVAL_MIN            200
#define DYNAMIC_KEYMAP_LAYER_COUNT  7
#define APC_KEYMAP_DOWN_LAYER       4
#define APC_KEYMAP_UP_LAYER         5
#define APC_KEYMAP_DKS_LAYER        6

#define USE_HS_USB
