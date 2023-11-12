/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021, Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#ifndef _DWC2_AT32_H_
#define _DWC2_AT32_H_

#ifdef __cplusplus
 extern "C" {
#endif

// EP_MAX       : Max number of bi-directional endpoints including EP0
// EP_FIFO_SIZE : Size of dedicated USB SRAM
#if CFG_TUSB_MCU == OPT_MCU_AT32F405
  #include "at32f402_405.h"
  #define USB_OTG_HS_PERIPH_BASE  OTGHS_BASE
  #define EP_MAX_HS               8
  #define EP_FIFO_SIZE_HS         4096
#else
  #error "Unsupported MCUs"
#endif

// OTG HS always has higher number of endpoints than FS
#ifdef USB_OTG_HS_PERIPH_BASE
  #define DWC2_EP_MAX   EP_MAX_HS
#else
  #define DWC2_EP_MAX   EP_MAX_FS
#endif

// - Only use OTG_HS
static const dwc2_controller_t _dwc2_controller[] =
{
#ifdef USB_OTG_HS_PERIPH_BASE
  { .reg_base = USB_OTG_HS_PERIPH_BASE, .irqnum = OTGHS_IRQn, .ep_count = EP_MAX_HS, .ep_fifo_size = EP_FIFO_SIZE_HS },
#endif
};

//--------------------------------------------------------------------+
//
//--------------------------------------------------------------------+

extern uint32_t SystemCoreClock;

TU_ATTR_ALWAYS_INLINE
static inline void dwc2_dcd_int_enable(uint8_t rhport)
{
  NVIC_EnableIRQ((IRQn_Type)_dwc2_controller[rhport].irqnum);
}

TU_ATTR_ALWAYS_INLINE
static inline void dwc2_dcd_int_disable (uint8_t rhport)
{
  NVIC_DisableIRQ((IRQn_Type)_dwc2_controller[rhport].irqnum);
}

TU_ATTR_ALWAYS_INLINE
static inline void dwc2_delay_1ms(void)
{
  // try to delay for 5 ms
  uint32_t count = (SystemCoreClock / 1000);
  while ( count-- ) __NOP();
}

TU_ATTR_ALWAYS_INLINE
static inline void dwc2_remote_wakeup_delay(void)
{
  // try to delay for 5 ms
  for (int i = 0; i < 5; i++) {
    dwc2_delay_1ms();
  }
}

// MCU specific PHY init, called BEFORE core reset
static inline void dwc2_phy_init(dwc2_regs_t * dwc2, uint8_t hs_phy_type)
{
  // Ignore VBUS
  dwc2->at32_gccfg |= AT32_GCCFG_VBUSIG;

  // Power on usb phy
  dwc2->at32_gccfg |= AT32_GCCFG_PWRDWN;
  dwc2_delay_1ms();
}

// MCU specific PHY update, it is called AFTER init() and core reset
static inline void dwc2_phy_update(dwc2_regs_t * dwc2, uint8_t hs_phy_type)
{
  uint32_t turnaround;
  if ( hs_phy_type == HS_PHY_TYPE_NONE )
  {
    turnaround = 5;
  } else {
    turnaround = 9;
  }
  dwc2->gusbcfg = (dwc2->gusbcfg & ~GUSBCFG_TRDT_Msk) | (turnaround << GUSBCFG_TRDT_Pos);
}

#ifdef __cplusplus
}
#endif

#endif /* _DWC2_AT32_H_ */
