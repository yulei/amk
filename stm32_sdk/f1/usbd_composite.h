/**
 * @file usbd_composite.h
 *  composite device handler
 */

#pragma once

#include "usbd_ioreq.h"
#include "usbd_hid.h"
#include "usbd_webusb.h"

extern USBD_ClassTypeDef USBD_COMP;

uint8_t USBD_COMP_Send(USBD_HandleTypeDef *pdev, uint8_t type, uint8_t *report, uint16_t len);