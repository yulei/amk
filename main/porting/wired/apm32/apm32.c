/**
 * @file gd32.c
 * @author astro
 * 
 * @copyright Copytight (c) 2023
 */

#include "amk_hal.h"
#include "cm_misc.h"
#include "wait.h"

#ifndef HSE_VALUE
#define HSE_VALUE   HXTAL_VALUE
#endif

extern void system_clock_init(void);

void system_init(void)
{
    NVIC_ConfigPriorityGroup(NVIC_PRIORITY_GROUP_4);

    system_clock_init();

#ifndef RTOS_ENABLE
    systick_init();
#endif

    dwt_delay_init();

#ifdef EECONFIG_FLASH 
extern void fee_init(void);
    fee_init();
#endif
}

//#define FAULT_BREAK
static void fault_handler(void) 
{
    #ifdef FAULT_BREAK
    __asm__("BKPT");
    #else
    NVIC_SystemReset();
    #endif
}

uint32_t magic_read(void)
{
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);
    PMU_EnableBackupAccess();
    return RTC_ReadBackup(RTC_BAKP_REG_0);
}

void magic_write(uint32_t magic)
{
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);
    PMU_EnableBackupAccess();

    RTC_WriteBackup(RTC_BAKP_REG_0, magic);
}

uint32_t reset_read(void)
{
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);
    PMU_EnableBackupAccess();
    return RTC_ReadBackup(RTC_BAKP_REG_1);
}

void reset_write(uint32_t reason)
{
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);
    PMU_EnableBackupAccess();

    RTC_WriteBackup(RTC_BAKP_REG_1, reason);
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

void DebugMon_Handler(void)
{
    fault_handler();
}
