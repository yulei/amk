/**
 * wait.c
 */

#include "stm32f4xx_hal.h"
#include "wait.h"

void wait_ms(int ms)
{
    HAL_Delay(ms);
}

void wait_us(int us)
{
    __IO uint32_t delay = us * 72 / 8;
    do {
        __NOP();
    } while (delay --);
}