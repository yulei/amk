/**
 * config.h
 *
 */
#pragma once

/* USB Device descriptor parameter */
#define VENDOR_ID       0x4D58   // MX
#define PRODUCT_ID      0x5246   // RF
#define DEVICE_VER      0x0001
#define MANUFACTURER    astro
#define PRODUCT         RF host

/* key matrix size */
#define MATRIX_ROWS 1
#define MATRIX_COLS 1

#define MATRIX_ROW_PINS { 9}
#define MATRIX_COL_PINS { 10}

// LEDS and BUTTON
/*#define LED1_G         NRF_GPIO_PIN_MAP(0,6)
#define LED2_R         NRF_GPIO_PIN_MAP(0,8)
#define LED2_G         NRF_GPIO_PIN_MAP(1,9)
#define LED2_B         NRF_GPIO_PIN_MAP(0,12)
#define LED_1          LED1_G
#define LED_2          LED2_R
#define LED_3          LED2_G
#define LED_4          LED2_B

#define LEDS_ACTIVE_STATE 0

#define BUTTON_1       NRF_GPIO_PIN_MAP(1,6)
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0
*/
//
// rf host do not need sleep
#define GZLL_IS_HOST 1
#define DISABLE_SLEEP

