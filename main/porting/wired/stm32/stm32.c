/**
 * stm32.c
 */

#include "amk_hal.h"
#include "wait.h"

#ifdef DWT_DELAY
#ifdef STM32L072xx
#error "STM32L0 do not have DWT"
#endif
#endif

static void dwt_delay_init(void)
{
#ifdef DWT_DELAY
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
#endif
}

void system_init(void)
{
    HAL_Init();

    dwt_delay_init();

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

#if !defined(STM32F722xx) 
#if !defined(RTOS_ENABLE)
void SysTick_Handler(void)
{
    HAL_IncTick();

    #ifdef SCREEN_ENABLE
    //screen_ticks(1);
    #endif
}
#endif
#endif

extern RTC_HandleTypeDef hrtc;

void magic_write(uint32_t magic)
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

void Error_Handler(void)
{
    __asm__("BKPT");
}
