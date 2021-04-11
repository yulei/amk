/**
 * bootloader.c
 *  jump to bootloader
 */

#include "generic_hal.h"
#include "bootloader.h"
#include "tusb.h"
#include "wait.h"

#if defined (NUC126)
static void magic_write(uint32_t magic)
{}
#else
extern RTC_HandleTypeDef hrtc;

static uint32_t Bootloader_Magic=0x41544B42;

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

    tud_disconnect();
    wait_ms(10);
    NVIC_SystemReset();
}