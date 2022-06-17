/**
 * @file dwc2_hc32.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#if CFG_TUSB_MCU == OPT_MCU_HC32F460
  #include "generic_hal.h"
  #include "tusb.h"
  #define EP_MAX_FS       8
  #define EP_FIFO_SIZE_FS 1280
#else
  #error "Unsupported MCUs"
#endif

#define DWC2_REG_BASE       0x400C0000 
#define DWC2_EP_MAX         EP_MAX_FS
#define DWC2_EP_FIFO_SIZE   EP_FIFO_SIZE_FS
#define RHPORT_IRQn         Int024_IRQn

extern uint32_t SystemCoreClock;

TU_ATTR_ALWAYS_INLINE
static inline void dwc2_dcd_int_enable(uint8_t rhport)
{
  (void) rhport;
  NVIC_EnableIRQ(RHPORT_IRQn);
}

TU_ATTR_ALWAYS_INLINE
static inline void dwc2_dcd_int_disable (uint8_t rhport)
{
  (void) rhport;
  NVIC_DisableIRQ(RHPORT_IRQn);
}

TU_ATTR_ALWAYS_INLINE
static inline void dwc2_remote_wakeup_delay(void)
{
  // try to delay for 1 ms
  uint32_t count = SystemCoreClock / 1000;
  while ( count-- ) __NOP();
}

// MCU specific PHY init, called BEFORE core reset
static inline void dwc2_phy_init(dwc2_regs_t * dwc2, uint8_t hs_phy_type)
{
  // usb clock config

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
  while(Set != CLK_GetFlagStatus(ClkFlagUPLLRdy)) {
    ;
  }
  /* Set USB clock source */
  CLK_SetUsbClkSource(ClkUsbSrcUpllp);

  // USB port config
  stc_port_init_t stcPortInit;
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

static inline void dwc2_irq_handler(void)
{
  dcd_int_handler(0);
}

// MCU specific PHY update, it is called AFTER init() and core reset
static inline void dwc2_phy_update(dwc2_regs_t * dwc2, uint8_t hs_phy_type)
{
  dwc2->gusbcfg = (dwc2->gusbcfg & ~GUSBCFG_TRDT_Msk) | (9u << GUSBCFG_TRDT_Pos);

  // irq setting
  stc_irq_regi_conf_t stcIrqRegiConf;
  /* Register INT_USBFS_GLB Int to Vect.No.024 */
  stcIrqRegiConf.enIRQn = Int024_IRQn;
  /* Select INT_USBFS_GLB interrupt function */
  stcIrqRegiConf.enIntSrc = INT_USBFS_GLB;
  /* Callback function */
  stcIrqRegiConf.pfnCallback = &dwc2_irq_handler;
  /* Registration IRQ */
  enIrqRegistration(&stcIrqRegiConf);
  /* Clear Pending */
  NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
  /* Set priority */
  NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
  /* Enable NVIC */
  NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
}
