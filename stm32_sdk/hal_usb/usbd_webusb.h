/**
 * @file usbd_webusb.h
 */

#pragma once

#include  "usbd_ioreq.h"

typedef enum {
    WEBUSB_IDLE = 0,
    WEBUSB_BUSY,
} WEBUSB_StateTypeDef;

#define WEBUSB_PACKET_SIZE  64
typedef struct {
    WEBUSB_StateTypeDef state;
    uint8_t recv_buffer[WEBUSB_PACKET_SIZE];
    uint8_t send_buffer[WEBUSB_PACKET_SIZE];
} USBD_WEBUSB_HandleTypeDef;

extern usbd_class_interface_t USBD_WEBUSB;
