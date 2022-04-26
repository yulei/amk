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
#define MATRIX_ROW_PINS {}
#define MATRIX_COL_PINS {}

#define ROW_1_PIN       B5
#define ROW_2_PIN       C10
#define ROW_3_PIN       A9
#define ROW_4_PIN       A10

#define LEFT_EN_PIN     B3
#define COL_A_PIN       D2
#define COL_B_PIN       C12
#define COL_C_PIN       C11

#define OPA_EN_PIN      A1
#define DISCHARGE_PIN   A2

#define KEY_IN_CHANNEL  ADC_CHANNEL_0
#define KEY_IN_PIN      GPIO_PIN_0
#define KEY_IN_PORT     GPIOA
#define SENSE_TH        1024

#define BUTTON_KEY      C9
#define BUTTON_ROW      0
#define NUM_LED_PIN     A8

#define USE_ADC1

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
//#define DEBOUNCE        5

#define VECT_TAB_OFFSET 0x20000

#define VIAL_INSECURE
#define VIAL_KEYBOARD_UID {0x86, 0x53, 0x6B, 0x6A, 0xEC, 0xB0, 0x3F, 0x5F}
