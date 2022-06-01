/**
 * config.h
 *
 */

#pragma once

#define VENDOR_ID       0x4D58   // MX
#define PRODUCT_ID      0x3858   // 8X
#define DEVICE_VER      0x0003
#define MANUFACTURER    Matrix Lab
#define PRODUCT         8XV3.0

// key matrix
#define MATRIX_ROWS         6
#define MATRIX_COLS         16
#define MATRIX_ROW_PINS     {}
#define MATRIX_COL_PINS     {}

#define ROW_1_PIN           A0
#define ROW_2_PIN           C15
#define ROW_3_PIN           C14
#define ROW_4_PIN           C13
#define ROW_5_PIN           B4
#define ROW_6_PIN           B10

#define LEFT_EN_PIN         B2
#define RIGHT_EN_PIN        A2
#define COL_A_PIN           A3
#define COL_B_PIN           B0
#define COL_C_PIN           B1

#define KEY_PIN             A1

#define USE_I2C1
#define IS31FL3741_I2C_ID   I2C_INSTANCE_1
#define I2C2_SCL_PIN        B8
#define I2C2_SDA_PIN        B9

#define WS2812_LED_NUM      7
#define WS2812_LED_PIN      B6

#define USE_PWM_TIM4
#define PWM_TIM_PERIOD      119
#define PWM_TIM             htim4
#define PWM_TIM_CHANNEL     TIM_CHANNEL_1
#define PWM_TIM_DMA_ID      TIM_DMA_ID_CC1
#define PWM_DMA_INSTANCE    DMA1_Stream0
#define PWM_DMA_CHANNEL     DMA_CHANNEL_2
#define PWM_DMA_IRQHnadler  DMA1_Stream0_IRQHandler

#define RGB_MATRIX_LED_NUM  97
#define RGB_LED_NUM         (WS2812_LED_NUM+RGB_MATRIX_LED_NUM)

#define RGB_DEVICE_NUM      2
#define RGB_SEGMENT_NUM     1
#define RGB_MATRIX_NUM      1

#define DEBOUNCE            10
//#define DWT_DELAY
#define VECT_TAB_OFFSET     0x20000
#define FAULT_BREAK

#define USE_SPI1
#define FLASH_CS            A4
#define W25QXX_SPI_ID       SPI_INSTANCE_1

// screen configuration
#define USE_SPI2
#define ST7735_SPI_ID       SPI_INSTANCE_2
#define SCREEN_NUM          1
#define SCREEN_0_PWR        A10
#define SCREEN_0_PWR_EN     0
#define SCREEN_0_CS         B12
#define SCREEN_0_RESET      A9
#define SCREEN_0_DC         A8

#define SCREEN_ROTATION     1
#define SCREEN_DRIVER_NUM   1
#define SCREEN_NUM          1
#define DISPLAY_NUM         1
#define ANIM_DISPLAY_NUM    1
//#define AUDIO_DISPLAY_NUM   1
#define RENDER_BUFFER_SIZE  (60*60*2)

// vial 
#define VIAL_INSECURE
#define VIAL_KEYBOARD_UID {0xD0, 0xD0, 0x10, 0x8F, 0x9D, 0x7A, 0x29, 0x92}

// custom tusb definitions
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

// RGB Matrix Animation modes. Explicitly enabled
// For full list of effects, see:
// https://docs.qmk.fm/#/feature_rgb_matrix?id=rgb-matrix-effects
#    define ENABLE_RGB_MATRIX_ALPHAS_MODS
#    define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
#    define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#    define ENABLE_RGB_MATRIX_BREATHING
#    define ENABLE_RGB_MATRIX_BAND_SAT
#    define ENABLE_RGB_MATRIX_BAND_VAL
#    define ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
#    define ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
#    define ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
#    define ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
#    define ENABLE_RGB_MATRIX_CYCLE_ALL
#    define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#    define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
#    define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
#    define ENABLE_RGB_MATRIX_CYCLE_OUT_IN
#    define ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
#    define ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
#    define ENABLE_RGB_MATRIX_CYCLE_SPIRAL
#    define ENABLE_RGB_MATRIX_DUAL_BEACON
#    define ENABLE_RGB_MATRIX_RAINBOW_BEACON
#    define ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS
#    define ENABLE_RGB_MATRIX_RAINDROPS
#    define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
#    define ENABLE_RGB_MATRIX_HUE_BREATHING
#    define ENABLE_RGB_MATRIX_HUE_PENDULUM
#    define ENABLE_RGB_MATRIX_HUE_WAVE
#    define ENABLE_RGB_MATRIX_PIXEL_RAIN
#    define ENABLE_RGB_MATRIX_PIXEL_FLOW
#    define ENABLE_RGB_MATRIX_PIXEL_FRACTAL

#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
// enabled only if RGB_MATRIX_FRAMEBUFFER_EFFECTS is defined
#    define ENABLE_RGB_MATRIX_TYPING_HEATMAP
#    define ENABLE_RGB_MATRIX_DIGITAL_RAIN

#define RGB_MATRIX_KEYPRESSES
// enabled only of RGB_MATRIX_KEYPRESSES or RGB_MATRIX_KEYRELEASES is defined
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
#    define ENABLE_RGB_MATRIX_SPLASH
#    define ENABLE_RGB_MATRIX_MULTISPLASH
#    define ENABLE_RGB_MATRIX_SOLID_SPLASH
#    define ENABLE_RGB_MATRIX_SOLID_MULTISPLASH
