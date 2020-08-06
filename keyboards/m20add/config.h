/**
 * config.h
 *
 */

#pragma once

/* USB Device descriptor parameter */
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x20AD   // 8XV2.0 Additional
#define DEVICE_VER 0x0001
#define MANUFACTURER MATRIX
#define PRODUCT 8XV2.0 Additional
#define DESCRIPTION Matrix 8XV2.0 Additional keyboard

/* key matrix size */
#define MATRIX_ROWS 6
#define MATRIX_COLS 16

#define MATRIX_ROW_PINS {MAKE_PIN(GPIOA, 0)}
#define MATRIX_COL_PINS {MAKE_PIN(GPIOB, 0)}