/**
 * stm32.c
 */

#include "generic_hal.h"
#include "wait.h"

#ifdef DWT_DELAY
#ifdef STM32L072xx
#error "STM32L0 do not have DWT"
#endif

static void dwt_delay_init(void)
{
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}
#endif

void system_init(void)
{
    HAL_Init();
#ifdef DWT_DELAY
    dwt_delay_init();
#endif
    //wait_ms(10000);
#ifdef EECONFIG_FLASH 
extern void fee_init(void);
    fee_init();
#endif
}

uint32_t systick_get_tick(void)
{
    return HAL_GetTick();
}

#if !defined(STM32F722xx)// && !defined(STM32F411xE)
void SysTick_Handler(void)
{
    HAL_IncTick();

    #ifdef SCREEN_ENABLE
    //screen_ticks(1);
    #endif
}
#endif

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
