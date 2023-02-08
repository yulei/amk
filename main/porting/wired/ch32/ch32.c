/**
 * @file gd32.c
 */

#include "generic_hal.h"
#include "cm_misc.h"
#include "wait.h"

static void usb_clock_init(void);
void system_init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    SystemCoreClockUpdate();
    usb_clock_init();

#ifndef RTOS_ENABLE
    systick_init();
#endif

    dwt_delay_init();

#ifdef EECONFIG_FLASH 
extern void fee_init(void);
    fee_init();
#endif
}

void magic_write(uint32_t magic)
{
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE );
    PWR_BackupAccessCmd( ENABLE );
    BKP_ClearFlag();
    uint16_t high = (uint16_t)((magic>>16)&0xFFFF);
	BKP_WriteBackupRegister(BKP_DR2, high);
    uint16_t low = (uint16_t)((magic)&0xFFFF);
	BKP_WriteBackupRegister(BKP_DR1, low);
}

void usb_clock_init(void)
{
    __disable_irq();
    RCC_USBCLK48MConfig(RCC_USBCLK48MCLKSource_USBPHY);
    RCC_USBHSPLLCLKConfig(RCC_HSBHSPLLCLKSource_HSE);
#if HSE_VALUE == 8000000
    RCC_USBHSConfig(RCC_USBPLL_Div2);
#else
    RCC_USBHSConfig(RCC_USBPLL_Div4);
#endif
    RCC_USBHSPLLCKREFCLKConfig(RCC_USBHSPLLCKREFCLK_4M);
    RCC_USBHSPHYPLLALIVEcmd(ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBHS, ENABLE);
    __enable_irq();
}