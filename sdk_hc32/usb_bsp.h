/**
 * @file usb_bsp.h
 * 
 */

#pragma once

#include "usb_core_driver.h"
#include "usb_app_conf.h"
#include "hc32_ddl.h"

void hd_usb_bsp_init(usb_core_instance *pdev);
void hd_usb_udelay(const uint32_t usec);
void hd_usb_mdelay(const uint32_t msec);
void hd_usb_bsp_nvicconfig (void);
