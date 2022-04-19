/**
 * @file hc32.c
 */

#include "generic_hal.h"
#include "gd32_util.h"
#include "wait.h"

#ifndef HSE_VALUE
#define HSE_VALUE   HXTAL_VALUE
#endif

static void dwt_delay_init(void);
//static void system_clock_init(void);
static void system_usb_init(void);

void system_init(void)
{
    //system_clock_init();

    SystemCoreClockUpdate();

    systick_init();

    dwt_delay_init();

#ifdef EECONFIG_FLASH 
extern void fee_init(void);
    fee_init();
#endif
    system_usb_init();
}

static void dwt_delay_init(void)
{
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}

void system_usb_init(void)
{}

void custom_board_init(void)
{}

void custom_board_task(void)
{}

#if 0
static void fault_handler(void) 
{
    #ifdef FAULT_BREAK
    __asm__("BKPT");
    #else
    NVIC_SystemReset();
    #endif
}

void NMI_Handler(void)
{
    fault_handler();
}

void HardFault_Handler(void)
{
    fault_handler();
}

void MemManage_Handler(void)
{
    fault_handler();
}

void BusFault_Handler(void)
{
    fault_handler();
}

void UsageFault_Handler(void)
{
    fault_handler();
}
#endif