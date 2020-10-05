/**
 * @file usbd_webusb.h
 */

#pragma once

#include  "usbd_ioreq.h"

typedef enum {
    WEBUSB_IDLE = 0,
    WEBUSB_BUSY,
} WEBUSB_StateTypeDef;

typedef struct {
    WEBUSB_StateTypeDef state;
} USBD_WEBUSB_HandleTypeDef;

extern usbd_class_interface_t USBD_WEBUSB;
