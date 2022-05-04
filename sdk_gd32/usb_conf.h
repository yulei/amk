/**
 * @file usb_conf.h
 */

#pragma once

#include <stdlib.h>
#include "gd32e10x.h"

#ifdef USE_USB_FS
    #define USB_FS_CORE
#endif

// for usb host
#ifdef USB_FS_CORE
    #define USB_RX_FIFO_FS_SIZE                            128
    #define USB_HTX_NPFIFO_FS_SIZE                         96
    #define USB_HTX_PFIFO_FS_SIZE                          96
#endif

// for usb device
#ifdef USB_FS_CORE
    #define RX_FIFO_FS_SIZE                                 128
    #define TX0_FIFO_FS_SIZE                                64
    #define TX1_FIFO_FS_SIZE                                16
    #define TX2_FIFO_FS_SIZE                                16
    #define TX3_FIFO_FS_SIZE                                96
#endif

//#define VBUS_SENSING_ENABLED

#define USB_SOF_OUTPUT                                     0
#define USB_LOW_POWER                                      0U
//#define USB_FS_LOW_PWR_ENABLE

//#define USE_HOST_MODE
//#define USE_DEVICE_MODE
//#define USE_OTG_MODE
#ifndef USE_DEVICE_MODE
    #ifndef USE_HOST_MODE
        #error "USE_DEVICE_MODE or USE_HOST_MODE should be defined"
    #endif
#endif
