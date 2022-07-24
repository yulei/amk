/**
 * config.h
 *
 */

#pragma once

// USB Device descriptor parameter
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x5044   // PD
#define DEVICE_VER 0x0003
#define MANUFACTURER Matrix Lab
#define PRODUCT 20Add Pad 

// key matrix
#define MATRIX_ROWS 6
#define MATRIX_COLS 4

#define MATRIX_DIRECT_PINS  {   B2,  C9, B13, B12, \
                                C3, A15,  C7,  C8, \
                                C2, C12, C11, C10, \
                                C1,  B5,  B4,  D2, \
                                C0,  B8,  B7,  B6, \
                               C15, C14, C13,  B9}

#define USE_I2C2 
#define IS31FL3731_I2C_ID   I2C_INSTANCE_2
#define I2C2_SCL_PIN        B10
#define I2C2_SDA_PIN        B11

#define WS2812_LED_NUM      4
#define WS2812_LED_PIN      A10
#define PWM_TIM             htim1
#define PWM_TIM_CHANNEL     TIM_CHANNEL_3
#define PWM_TIM_PERIOD      105

#define RGB_MATRIX_LED_NUM  26
#define RGB_LED_NUM         30

#define RGB_DEVICE_NUM      2
#define RGB_SEGMENT_NUM     1
#define RGB_MATRIX_NUM      1

#define VECT_TAB_OFFSET     0x20000

#define VIAL_INSECURE
#define VIAL_KEYBOARD_UID {0xE6, 0x5A, 0xDA, 0x5E, 0xFF, 0x9F, 0xF9, 0xFF}