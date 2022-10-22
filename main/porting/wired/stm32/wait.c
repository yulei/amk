/**
 * wait.c
 */

#include "generic_hal.h"
#include "wait.h"

#ifdef RTOS_ENABLE
#include "cmsis_os2.h"
static void busy_delay_ms(uint32_t ms)
{
    while (ms--) {
        wait_us(1000);
    }
}

#endif

void wait_ms(int ms)
{
#ifdef RTOS_ENABLE
    if (osKernelGetState() == osKernelRunning) {
        osDelay(ms);
    } else {
        busy_delay_ms(ms);
    }
#else
    HAL_Delay(ms);
#endif
}

#ifdef DWT_DELAY
void wait_us(int us)
{
    __IO uint32_t startTick = DWT->CYCCNT;
    __IO uint32_t delayTicks = us * (SystemCoreClock/1000000);

    while (DWT->CYCCNT - startTick < delayTicks) {};
}
#else
void wait_us(int us)
{
    __IO uint32_t ticks = us * (SystemCoreClock/1000000);
    while (ticks--) {};
}
#endif