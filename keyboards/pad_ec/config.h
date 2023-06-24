/**
 * config.h
 *
 */

#pragma once

// USB Device descriptor parameter
#define VENDOR_ID       0x4543  // EC
#define PRODUCT_ID      0x5044  // PD
#define DEVICE_VER      0x0003
#define MANUFACTURER    Matrix Lab
#define PRODUCT         Pad EC

// key matrix
#define MATRIX_ROWS     5
#define MATRIX_COLS     5

#define LEFT_MASK       0x08
#define RIGHT_MASK      0x10
#define MATRIX_ROW_PINS {B5, C10, A9, A10, C0}
#define MATRIX_COL_PINS {LEFT_MASK|2, LEFT_MASK|1, LEFT_MASK|0, LEFT_MASK|6, LEFT_MASK|4};

#define DISCHARGE_WAIT_PRE  20
#define DISCHARGE_WAIT_POST 20
#define EC_TH_LOW           600
#define EC_TH_HIGH          800

#define LEFT_EN_PIN     B3
#define COL_A_PIN       D2
#define COL_B_PIN       C12
#define COL_C_PIN       C11

#define OPA_EN_PIN      A1
#define DISCHARGE_PIN   A2

#define KEY_IN_CHANNEL  ADC_CHANNEL_0
#define KEY_IN_PIN      GPIO_PIN_0
#define KEY_IN_PORT     GPIOA
#define SENSE_TH        896

#define BUTTON_KEY      C9
#define BUTTON_ROW      0
#define NUM_LED_PIN     A8

#define USE_ADC1

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
//#define DEBOUNCE        5

#define VECT_TAB_OFFSET 0x20000

#define VIAL_KEYBOARD_UID {0x86, 0x53, 0x6B, 0x6A, 0xEC, 0xB0, 0x3F, 0x5F}
