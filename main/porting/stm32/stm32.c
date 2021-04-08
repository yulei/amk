/**
 * stm32.c
 */

#include "generic_hal.h"

static void DWT_Delay_Init(void);
extern void fee_init(void);

void system_init(void)
{
    HAL_Init();
    DWT_Delay_Init();
    fee_init();
}

static void DWT_Delay_Init(void)
{
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
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