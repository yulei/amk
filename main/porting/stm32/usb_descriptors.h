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
} report_id_t ;