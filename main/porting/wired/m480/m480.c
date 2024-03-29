/**
 * @file m480.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "generic_hal.h"
#include "wait.h"

static void dwt_delay_init(void);
static void system_clock_init(void);
static void system_usb_init(void);

void system_init(void)
{
    system_clock_init();

    SystemCoreClockUpdate();

    SysTick_Config(SystemCoreClock / 1000);

    dwt_delay_init();

#ifdef EECONFIG_FLASH 
extern void fee_init(void);
    fee_init();
#endif
    system_usb_init();
}

static void dwt_delay_init(void)
{
#ifdef DWT_DELAY
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
#endif
}

static void system_clock_init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

    /* Enable External XTAL (4~24 MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Switch HCLK clock source to HXT */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT,CLK_CLKDIV0_HCLK(1));

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(FREQ_192MHZ);

    /* Set both PCLK0 and PCLK1 as HCLK/2 */
    CLK->PCLKDIV = CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2;
}

void system_usb_init(void)
{
    /* Select HSUSBD */
    SYS->USBPHY &= ~SYS_USBPHY_HSUSBROLE_Msk;
    /* Enable USB PHY */
    SYS->USBPHY = (SYS->USBPHY & ~(SYS_USBPHY_HSUSBROLE_Msk | SYS_USBPHY_HSUSBACT_Msk)) | SYS_USBPHY_HSUSBEN_Msk;

    volatile uint32_t i;

    for (i = 0; i < 0x1000; i++); // delay > 10 us

    SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;
    /* Enable HSUSBD clock */
    CLK_EnableModuleClock(HSUSBD_MODULE);
}

void custom_board_task(void)
{}

#if 0
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
#endif

volatile uint32_t g_systick_counter = 0;

void SysTick_Handler(void)
{
    g_systick_counter ++;
}

uint32_t systick_get_tick(void)
{
    return g_systick_counter;
}

void systick_delay(uint32_t ticks)
{
    uint32_t tick_start = systick_get_tick();
    uint32_t wait = ticks;

    /* Add a freq to guarantee minimum wait */
    if (wait < 0xFFFFFFFFUL)
    {
        wait += 1;
    }

    while((systick_get_tick() - tick_start) < wait) {
        ;
    }
}
