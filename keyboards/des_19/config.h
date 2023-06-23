/**
 * @file config.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#define VENDOR_ID       0x4543      // EC
#define PRODUCT_ID      0xDE19      // DES 19
#define DEVICE_VER      0x0001
#define MANUFACTURER    DESKEYS
#define PRODUCT         NEVEREST 19

#define MATRIX_ROWS     4
#define MATRIX_COLS     5
#define L_MASK          0x08
#define R_MASK          0x10

#define MATRIX_ROW_PINS {B5, B4, B3, A15}
#define MATRIX_COL_PINS {L_MASK|2, L_MASK|4, L_MASK|5, L_MASK|6, L_MASK|7};

#define DISCHARGE_WAIT_PRE  20
#define DISCHARGE_WAIT_POST 20
#define EC_TH_LOW       1680
#define EC_TH_HIGH      2368

#define LEFT_EN_PIN     A7
#define COL_C_PIN       A6
#define COL_B_PIN       A5
#define COL_A_PIN       A4

//#define POWER_PIN       C14
#define DISCHARGE_PIN   C15

#define NUM_LED_PIN     B7

#define USE_ADC1
#define ADC_INST        hadc1
#define KEY_IN_CHANNEL  ADC_CHANNEL_5
#define KEY_IN_PIN      GPIO_PIN_0
#define KEY_IN_PORT     GPIOA

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE        5
#define DWT_DELAY

#define VECT_TAB_OFFSET 0x10000

#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"

//#define VIAL_INSECURE
#define VIAL_KEYBOARD_UID {0x85, 0x00, 0x86, 0x38, 0x69, 0x34, 0x17, 0x3F}

#define SUSPEND_RECONNECT
