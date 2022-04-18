/**
 * @file usb_app_conf.h
 * 
 */

#pragma once

#define RX_FIFO_FS_SIZE       0x80    // shared rx buffer
#define TX0_FIFO_FS_SIZE      0x20    // ep0 tx buffer
#define TX1_FIFO_FS_SIZE      0x10    // ep1 (keyboard) tx buffer
#define TX2_FIFO_FS_SIZE      0x10    // ep2 (mouse, system, consumer) tx buffer
#define TX3_FIFO_FS_SIZE      0x80    // ep3 (webusb or msc) tx buffer
#define TX4_FIFO_FS_SIZE      0u
#define TX5_FIFO_FS_SIZE      0u

#define USE_DEVICE_MODE
