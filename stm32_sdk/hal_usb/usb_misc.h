/**
 * @file usb_misc.h
 */

#pragma once

#include <stdint.h>
#include "usb_device.h"

typedef uint8_t (*setup_t)(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req, void* user); 
typedef uint8_t (*datain_t)(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user);
typedef uint8_t (*dataout_t)(USBD_HandleTypeDef *pdev, uint8_t epnum, void* user);
typedef uint8_t (*write_t)(USBD_HandleTypeDef *pdev, uint8_t epnum, uint8_t* data, uint16_t size, void* user);

typedef struct {
    setup_t Setup;
    datain_t DataIn;
    dataout_t DataOut;
    write_t Write;
} usbd_class_interface_t;
