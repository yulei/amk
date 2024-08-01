/**
 * @file lpc55.c
 * @author astro
 * 
 * @copyright Copytight (c) 2024
 */

#include "amk_hal.h"
#include "cm_misc.h"
#include "wait.h"

extern void system_clock_init(void);

void system_init(void)
{
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

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
    crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
    pwc_battery_powered_domain_access(TRUE);
    return ertc_bpr_data_read(ERTC_DT1);
}

void magic_write(uint32_t magic)
{
    crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
    pwc_battery_powered_domain_access(TRUE);
    ertc_bpr_data_write(ERTC_DT1, magic);
}

uint32_t reset_read(void)
{
    crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
    pwc_battery_powered_domain_access(TRUE);
    return ertc_bpr_data_read(ERTC_DT2);
}

void reset_write(uint32_t reset)
{
    crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
    pwc_battery_powered_domain_access(TRUE);
    ertc_bpr_data_write(ERTC_DT2, reset);
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
