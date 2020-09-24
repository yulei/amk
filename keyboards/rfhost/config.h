/**
 * config.h
 *
 */
#pragma once

/* USB Device descriptor parameter */
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x5246   // RF
#define DEVICE_VER 0x0001
#define MANUFACTURER astro
#define PRODUCT RF host receiver

/* key matrix size */
#define MATRIX_ROWS 1
#define MATRIX_COLS 1

//
// rf host do not need sleep
#define GZLL_IS_HOST 1
#define DISABLE_SLEEP