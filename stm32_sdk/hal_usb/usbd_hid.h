/**
 * @file usbd_hid.h
 */

#pragma once

#include "usbd_ioreq.h"
#include "usb_misc.h"
typedef enum {
    HIDD_IDLE = 0,
    HIDD_BUSY,
} HIDD_StateTypeDef;


typedef struct {
    uint32_t             Protocol;
    uint32_t             IdleState;
    uint32_t             AltSetting;
    uint32_t             IsKeyboard;
    HIDD_StateTypeDef     state;
} USBD_HID_HandleTypeDef;

extern usbd_class_interface_t USBD_HID;