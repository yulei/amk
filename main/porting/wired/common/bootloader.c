/**
 * bootloader.c
 *  jump to bootloader
 */

#include "generic_hal.h"
#include "bootloader.h"
#include "wait.h"
#include "amk_keymap.h"
#include "usb_interface.h"

static uint32_t Bootloader_Magic=0x41544B42;

#if defined (NUC126) || defined(__SAMD21G18A__) || defined(STM32L432xx) || defined(GD32E10X)
static void magic_write(uint32_t magic)
{}
#else
extern RTC_HandleTypeDef hrtc;

static void magic_write(uint32_t magic)
{
    HAL_PWR_EnableBkUpAccess();
#ifdef STM32F103xB
    uint16_t low = magic & 0xFFFF;
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, low);
    uint16_t high = (magic & 0xFFFF0000) >> 16;
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, high);
#else
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, magic);
#endif
}
#endif

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