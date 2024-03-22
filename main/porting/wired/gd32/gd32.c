/**
 * @file gd32.c
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

//#define FAULT_BREAK
static void fault_handler(void) 
{
    #ifdef FAULT_BREAK
    __asm__("BKPT");
    #else
    NVIC_SystemReset();
    #endif
}

static uint32_t gd32_bkp_read(uint32_t high, uint32_t low)
{
    rcu_periph_clock_enable(RCU_PMU);
    rcu_periph_clock_enable(RCU_BKPI);
    pmu_backup_write_enable();
    uint32_t high_half = bkp_read_data(high);
    uint32_t low_half = bkp_read_data(low);

    return ((high_half << 16) | low_half);
}

static void gd32_bkp_write(uint32_t magic, uint32_t high, uint32_t low)
{
    rcu_periph_clock_enable(RCU_PMU);
    rcu_periph_clock_enable(RCU_BKPI);
    pmu_backup_write_enable();
    uint16_t high_half = (uint16_t)((magic>>16)&0xFFFF);
    bkp_write_data(high, high_half);
    uint16_t low_half = (uint16_t)((magic)&0xFFFF);
    bkp_write_data(low, low_half);
}

uint32_t magic_read(void)
{
    return gd32_bkp_read(BKP_DATA_1, BKP_DATA_0);
}

void magic_write(uint32_t magic)
{
    gd32_bkp_write(magic, BKP_DATA_1, BKP_DATA_0);
}

uint32_t reset_read(void)
{
    return gd32_bkp_read(BKP_DATA_3, BKP_DATA_2);
}

void reset_write(uint32_t reason)
{
    gd32_bkp_write(reason, BKP_DATA_3, BKP_DATA_2);
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
