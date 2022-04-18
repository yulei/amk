/**
 * @file ddl_usb.c
 * 
 */

#include "hc32_ddl.h"
#include "usb_core_driver.h"
#include "tusb.h"
#include "amk_printf.h"

#ifndef DDLUSB_DEBUG
#define DDLUSB_DEBUG 0
#endif

#if DDLUSB_DEBUG
#define ddlusb_debug  amk_printf
#else
#define ddlusb_debug(...)
#endif

extern  usb_core_instance       usb_dev;

//extern  void hd_usb_isr_handler (usb_core_instance *pdev);

void USB_IRQ_Handler(void)
{
    //hd_usb_isr_handler(&usb_dev);
    tud_int_handler(0);
}

static void ddl_usb_clock_init(void)
{
    // UPLL clock = ((XTAL/pllm) * (plln)) / (pllp)
    // we need 48M for the USB
    stc_clk_upll_cfg_t      stcUpllCfg;

    stcUpllCfg.pllmDiv = XTAL_VALUE/1000000;
    stcUpllCfg.plln = 336u;
    stcUpllCfg.PllpDiv = 7u;
    stcUpllCfg.PllqDiv = 7u;
    stcUpllCfg.PllrDiv = 7u;
    CLK_UpllConfig(&stcUpllCfg);
    CLK_UpllCmd(Enable);
    /* Wait UPLL ready. */
    while(Set != CLK_GetFlagStatus(ClkFlagUPLLRdy))
    {
        ;
    }

    /* Set USB clock source */
    CLK_SetUsbClkSource(ClkUsbSrcUpllp);
}

// called by the usb lib 
void hd_usb_bsp_init(usb_core_instance *pdev)
{
    stc_port_init_t stcPortInit;

    /* clock config */
    ddl_usb_clock_init();

    /* port config */
    /* Disable digital function for DM DP */
    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_Ana;
    PORT_Init(PortA, Pin11, &stcPortInit);
    PORT_Init(PortA, Pin12, &stcPortInit);
    //PORT_SetFunc(PortA, Pin09, Func_UsbF, Disable); //VBUS
    PORT_SetFunc(PortA, Pin11, Func_UsbF, Disable); //DM
    PORT_SetFunc(PortA, Pin12, Func_UsbF, Disable); //DP

    PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_USBFS, Enable);
}

void hd_usb_bsp_nvicconfig(void)
{
    stc_irq_regi_conf_t stcIrqRegiConf;
    /* Register INT_USBFS_GLB Int to Vect.No.024 */
    stcIrqRegiConf.enIRQn = Int024_IRQn;
    /* Select INT_USBFS_GLB interrupt function */
    stcIrqRegiConf.enIntSrc = INT_USBFS_GLB;
    /* Callback function */
    stcIrqRegiConf.pfnCallback = &USB_IRQ_Handler;
    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);
    /* Clear Pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
}

#define Fclk SystemCoreClock
void hd_usb_udelay(const uint32_t usec)
{
    __IO uint32_t i;
    uint32_t j = Fclk / 1000000ul * usec;
    for(i = 0ul; i < j; i++)
    {
        ;
    }
}

void hd_usb_mdelay (const uint32_t msec)
{
    hd_usb_udelay(msec * 1000ul);
}

