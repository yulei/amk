/**
 * @file usbd_hid.h
 */

#pragma once

#include  "usbd_ioreq.h"

typedef enum {
    HID_IDLE = 0,
    HID_BUSY,
} HID_StateTypeDef;


typedef struct {
    uint32_t             Protocol;
    uint32_t             IdleState;
    uint32_t             AltSetting;
    uint32_t             IsKeyboard;
    HID_StateTypeDef     State;
} USBD_HID_HandleTypeDef;

extern usbd_class_interface_t USBD_HID;