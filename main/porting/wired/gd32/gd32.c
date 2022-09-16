/**
 * @file gd32.c
 */

#include "generic_hal.h"
#include "cm_misc.h"
#include "wait.h"

#ifndef HSE_VALUE
#define HSE_VALUE   HXTAL_VALUE
#endif

extern void system_clock_init(void);

void system_init(void)
{
    NVIC_SetPriorityGrouping(NVIC_PRIGROUP_PRE4_SUB0);

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

#define FAULT_BREAK
static void fault_handler(void) 
{
    #ifdef FAULT_BREAK
    __asm__("BKPT");
    #else
    NVIC_SystemReset();
    #endif
}

void magic_write(uint32_t magic)
{
    rcu_periph_clock_enable(RCU_PMU);
    rcu_periph_clock_enable(RCU_BKPI);
    pmu_backup_write_enable();
    uint16_t high = (uint16_t)((magic>>16)&0xFFFF);
    bkp_write_data(BKP_DATA_1, high);
    uint16_t low = (uint16_t)((magic)&0xFFFF);
    bkp_write_data(BKP_DATA_0, low);
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
