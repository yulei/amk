/**
 * bootloader.c
 *  reset usb to bootloader mode
 */

#include "bootloader.h"
#include "usb_interface.h"

void bootloader_jump(void)
{
    nrf_usb_reboot();
}