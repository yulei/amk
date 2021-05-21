/**
 * @file usb_comp.h
 * @brief hid composite device
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "usbd_enum.h"
#include "usb_hid.h"

extern usb_desc comp_desc;
extern usb_class_core usbd_comp_cb;

bool usbd_comp_itf_ready(usb_core_driver *udev, uint32_t itf);
uint8_t usbd_comp_send(usb_core_driver *udev, uint8_t type, uint8_t *report, uint16_t len);
