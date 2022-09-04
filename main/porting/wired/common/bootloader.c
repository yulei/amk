/**
 * bootloader.c
 *  jump to bootloader
 */

#include "generic_hal.h"
#include "bootloader.h"
#include "wait.h"
#include "amk_keymap.h"
#include "usb_interface.h"

const uint32_t Bootloader_Magic = 0x41544B42;

extern void magic_write(uint32_t magic);

void bootloader_jump(void)
{
    magic_write(Bootloader_Magic);
    amk_keymap_reset();
    amk_keymap_macro_reset();

#if !defined(USB_HOST_ENABLE) && !defined(GD32E10X)
    usb_connect(0);
#endif
    wait_ms(10);
    NVIC_SystemReset();
}
