/*!
    \file    gd32e10x_it.c
    \brief   main interrupt service routines

    \version 2020-08-05, V2.0.0, firmware for GD32E10x
    \version 2020-12-31, V2.1.0, firmware for GD32E10x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc. 

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "drv_usb_hw.h"
#include "gd32e10x_it.h"
#include "gd32_util.h"

#ifdef USB_HOST_ENABLE
#include "drv_usbh_int.h"
extern usb_core_driver usbh_core;
#endif

#ifdef USB_DEVICE_ENABLE
#include "drv_usbd_int.h"
extern usb_core_driver usbd_core;
#endif

#if USB_LOW_POWER
/* local function prototypes ('static') */
static void resume_mcu_clk(void);
#endif /* USB_LOW_POWER */

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    systick_inc_tick();
}

#if USB_LOW_POWER

/*!
    \brief      this function handles external line 0 interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI0_IRQHandler(void)
{
    if (exti_interrupt_flag_get(WAKEUP_KEY_EXTI_LINE) != RESET) {
        if (usb_host.suspend_flag == 1) {
            usb_host.suspend_flag = 0;
            usb_host.wakeup_mode = 1; /* general wakeup mode */

            /* configure system clock */
            resume_mcu_clk();
        }

        /* clear the EXTI line pending bit */
        exti_interrupt_flag_clear(WAKEUP_KEY_EXTI_LINE);
    }
}

#endif /* USB_LOW_POWER */
/*!
    \brief      this function handles external line 1 interrupt exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI1_IRQHandler(void)
{
}


/*!
    \brief      this function handles USBFS interrupt request 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_IRQHandler(void)
{
#ifdef USB_HOST_ENABLE
    usbh_isr(&usbh_core);
#endif

#ifdef USB_DEVICE_ENABLE
    //usbd_isr(&usbd_core);
    extern void dcd_int_handler(uint8_t rhport);
    dcd_int_handler(0);
#endif
}

#if USB_LOW_POWER

/*!
    \brief      this function handles USBHS wakeup interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_WKUP_IRQHandler(void)
{
    if (usb_host.suspend_flag == 1) {
        usb_host.suspend_flag = 0;
        usb_host.wakeup_mode = 2; /* remote wakeup mode */

        /* configure system clock */
        resume_mcu_clk();
    }

    exti_interrupt_flag_clear(EXTI_18);
}

/*!
    \brief      resume mcu clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void resume_mcu_clk(void)
{
    /* enable HSE */
    rcu_osci_on(RCU_HXTAL);

    /* wait till HSE is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_HXTALSTB)){
    }

    /* enable PLL1 */
    rcu_osci_on(RCU_PLL1_CK);

    /* wait till PLL1 is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_PLL1STB)){
    }

    /* enable PLL */
    rcu_osci_on(RCU_PLL_CK);

    /* wait till PLL is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_PLLSTB)){
    }

    /* select PLL as system clock source */
    rcu_system_clock_source_config(RCU_CKSYSSRC_PLL);

    /* wait till PLL is used as system clock source */
    while(RCU_SCSS_PLL != rcu_system_clock_source_get()){
    }
}

#endif /* USB_LOW_POWER */
