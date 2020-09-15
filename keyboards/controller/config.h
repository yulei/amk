/**
 * config.h
 *
 */
#pragma once

/* USB Device descriptor parameter */
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x4848   // HH
#define DEVICE_VER 0x0001
#define MANUFACTURER astro
#define PRODUCT HHKB BLE

/* key matrix size */
#define MATRIX_ROWS 8
#define MATRIX_COLS 8

#define VBUS_DETECT_PIN             4
#define UART_RX_PIN                 3
#define UART_TX_PIN                 2
#define BATTERY_SAADC_ENABLE_PIN    27
#define BATTERY_SAADC_PIN           NRF_SAADC_INPUT_AIN4

#define HHKB_POWER_EN               5

#define HHKB_KEY_IN                 20
#define HHKB_HYS                    19
#define HHKB_HC_A                   18
#define HHKB_HC_B                   17
#define HHKB_HC_C                   16
#define HHKB_LS_A                   15
#define HHKB_LS_B                   14
#define HHKB_LS_C                   13
#define HHKB_LS_D                   12

// because hhkb need active scanning, just disable sleep currently 
// because the power saving mode for hhkb was not implemented yet
#define DISABLE_SLEEP