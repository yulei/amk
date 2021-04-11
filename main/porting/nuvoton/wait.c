/**
 * wait.c
 */

#include "generic_hal.h"
#include "wait.h"

extern uint32_t systick_get_tick(void);
void wait_ms(int ms)
{
    if (ms == 0)
        return ;

    uint32_t end = systick_get_tick() + ms;
    while (systick_get_tick() < end)
        ;
}

void wait_us(int us)
{
    if (us == 0) return;

    uint32_t n = us * ((SystemCoreClock/ 1000000)+2) / 3;

    __asm__ __volatile__(
        "1:              \n"
        "   sub %0, #1   \n" // substract 1 from %0 (n)
        "   bne 1b       \n" // if result is not 0 jump to 1
        : "+r" (n)           // '%0' is n variable with RW constraints
        :                    // no input
        :                    // no clobber
    );
}