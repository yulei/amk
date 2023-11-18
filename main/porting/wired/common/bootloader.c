/**
 * bootloader.c
 *  jump to bootloader
 */

#include "amk_hal.h"
#include "cm_misc.h"
#include "bootloader.h"
#include "wait.h"
#include "usb_interface.h"

const uint32_t Bootloader_Magic = 0x41544B42;

void bootloader_jump(void)
{
    magic_write(Bootloader_Magic);
    //amk_keymap_reset();
    //amk_keymap_macro_reset();

#if !defined(USB_HOST_ENABLE) && !defined(GD32E10X)
    usb_connect(0);
#endif
    wait_ms(10);
    NVIC_SystemReset();
}

void mcu_reset(void)
{
    NVIC_SystemReset();
}
