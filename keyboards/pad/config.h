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

#define RGB_MATRIX_LED_NUM  26
#define IS31_DRIVER_NUM     1
#define IS31_ADDR1          0xE8
#define IS31_LED_NUM1       26

#define I2C_USE_INSTANCE_1
#define I2C1_SCL_PIN        B9
#define I2C1_SDA_PIN        B10

#define RGB_LED_NUM         4
#define WS2812_LED_PIN      A10
#define PWM_TIM_PERIOD      105

#define VECT_TAB_OFFSET         0x20000
