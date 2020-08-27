/**
 *  usb_descriptors.h
 */ 

#pragma once

#include "usb_def.h"

typedef enum {
  REPORT_ID_KEYBOARD = 1,
  REPORT_ID_MOUSE,
  REPORT_ID_SYSTEM,
  REPORT_ID_CONSUMER,
} report_id_t;

typedef enum {
  VENDOR_REQUEST_WEBUSB = 1,
  VENDOR_REQUEST_MICROSOFT = 2
} vender_request_t;

extern uint8_t const desc_ms_os_20[];