/**
 * @file mscusb.h
 *  usb msc interface
 */

#pragma once

void msc_init(void);
void msc_task(void);

// reset the chip to original state
void msc_erase(void);
