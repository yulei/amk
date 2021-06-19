/**
 * stm32.c
 */

#include "generic_hal.h"

static void DWT_Delay_Init(void);

void system_init(void)
{
    HAL_Init();
    DWT_Delay_Init();
#ifdef EECONFIG_FLASH 
extern void fee_init(void);
    fee_init();
#endif
}

static void DWT_Delay_Init(void)
{
#ifndef STM32L072xx
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
#endif
}


uint32_t systick_get_tick(void)
{
    return HAL_GetTick();
}

void SysTick_Handler(void)
{
    HAL_IncTick();

    #ifdef SCREEN_ENABLE
    //screen_ticks(1);
    #endif
}

static void fault_handler(void) 
{
    NVIC_SystemReset();
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
