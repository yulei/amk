/**
 * stm32.c
 */

#include <stdint.h>
#include "generic_hal.h"
#include "tusb.h"

void clock_init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    //CLK->PWRCTL = (CLK->PWRCTL & ~(0x0000200Ful)) | 0x00003C1Cul;
    //CLK->PLLCTL = (CLK->PLLCTL & ~(0x000FFFFFul)) | 0x0088400Bul;
    //CLK->CLKDIV0 = (CLK->CLKDIV0 & ~(0x00FF0FFFul)) | 0x00000000ul;
    //CLK->CLKDIV1 = (CLK->CLKDIV1 & ~(0x0000FFFFul)) | 0x00000000ul;
    //CLK->CLKSEL0 = (CLK->CLKSEL0 & ~(0x000000FFul)) | 0x0000003Aul;
    //CLK->CLKSEL1 = (CLK->CLKSEL1 & ~(0x3377770Ful)) | 0x3377770Ful;
    //CLK->CLKSEL2 = (CLK->CLKSEL2 & ~(0x0F0B001Cul)) | 0x00020008ul;
    //CLK->CLKSEL3 = (CLK->CLKSEL3 & ~(0x0000010Ful)) | 0x00000000ul;
    //CLK->AHBCLK = (CLK->AHBCLK & ~(0x003F809Eul)) | 0x003F8004ul;
    //CLK->APBCLK0 = (CLK->APBCLK0 & ~(0x5837337Ful)) | 0x08000000ul;
    //CLK->APBCLK1 = (CLK->APBCLK1 & ~(0x00000703ul)) | 0x00000000ul;
    //CLK->CLKOCTL = (CLK->CLKOCTL & ~(0x0000007Ful)) | 0x00000000ul;
    //SysTick->CTRL = (SysTick->CTRL & ~(0x00000005ul)) | 0x00000001ul;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable clock source */
    CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk|CLK_PWRCTL_HIRCEN_Msk|CLK_PWRCTL_HIRC48EN_Msk);

    /* Waiting for clock source ready */
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk|CLK_STATUS_HIRCSTB_Msk|CLK_STATUS_HIRC48STB_Msk);

    /* Disable PLL first to avoid unstable when setting PLL */
    CLK_DisablePLL();

    /* Set PLL frequency */
    CLK->PLLCTL = (CLK->PLLCTL & ~(0x000FFFFFul)) | 0x0088400Bul;

    /* Waiting for PLL ready */
    CLK_WaitClockReady(CLK_STATUS_PLLSTB_Msk);

    /* If the defines do not exist in your project, please refer to the related clk.h in the Header folder appended to the tool package. */
    /* Set HCLK clock */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_PLL, CLK_CLKDIV0_HCLK(1));

    /* Set PCLK-related clock */
    CLK->CLKSEL0 = CLK->CLKSEL0 & ~CLK_CLKSEL0_PCLK0SEL_Msk;
    CLK->CLKSEL0 = CLK->CLKSEL0 & ~CLK_CLKSEL0_PCLK1SEL_Msk;

    /* Enable IP clock */
    CLK_EnableModuleClock(FMCIDLE_MODULE);
    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);
    CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(GPIOD_MODULE);
    CLK_EnableModuleClock(GPIOE_MODULE);
    CLK_EnableModuleClock(GPIOF_MODULE);
    CLK_EnableModuleClock(ISP_MODULE);
    CLK_EnableModuleClock(USBD_MODULE);
    CLK_EnableSysTick(CLK_CLKSEL0_STCLKSEL_HIRC_DIV2, 0);

    /* Set IP clock */
    CLK_SetModuleClock(USBD_MODULE, CLK_CLKSEL3_USBDSEL_HIRC48, MODULE_NoMsk);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    SysTick_Config(SystemCoreClock/1000);

    /* Lock protected registers */
    SYS_LockReg();
}

void custom_board_init(void)
{
}

void custom_board_task(void)
{
}

void system_init(void)
{
    clock_init();
}

static volatile uint32_t _dwTickCount = 0;
void SysTick_Handler(void)
{
    _dwTickCount++;
}

uint32_t systick_get_tick(void)
{
    return _dwTickCount;
}

void USBD_IRQHandler(void)
{
    tud_int_handler(0);
}
