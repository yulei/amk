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


#ifndef DWC2_GD32_H_
#define DWC2_GD32_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "gd32e50x.h"

#define DWC2_REG_BASE       0x50000000UL
#define DWC2_EP_MAX         6
#define DWC2_EP_FIFO_SIZE   4096
#define RHPORT_IRQn         USBHS_IRQn

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

static inline void dwc2_delay_ms(uint32_t ms)
{
  // try to delay for 1 ms
  uint32_t count = SystemCoreClock / 1000;
  while (ms--) {
    uint32_t num = count;
    while (num--) __NOP();
  }
}

TU_ATTR_ALWAYS_INLINE
static inline void dwc2_remote_wakeup_delay(void)
{
  dwc2_delay_ms(1);
}

// MCU specific PHY init, called BEFORE core reset
static inline void dwc2_phy_init(dwc2_regs_t * dwc2, uint8_t hs_phy_type)
{
  (void) dwc2;
  (void) hs_phy_type;

  // do nothing
}

// MCU specific PHY update, it is called AFTER init() and core reset
static inline void dwc2_phy_update(dwc2_regs_t * dwc2, uint8_t hs_phy_type)
{
  (void) dwc2;
  (void) hs_phy_type;
  // power on 
  //dwc2->gd32_gccfg = 0;
  dwc2->gd32_gccfg |= GD32_GCCFG_PWRON;

  // Set turn-around
  // - 9 if high speed
  uint32_t gusbcfg  = dwc2->gusbcfg;
  gusbcfg &= ~GUSBCFG_TRDT_Msk;
  gusbcfg |= 9u << GUSBCFG_TRDT_Pos;
  dwc2->gusbcfg = gusbcfg;

  dwc2_delay_ms(20);
}

#ifdef __cplusplus
}
#endif

#endif /* DWC2_GD32_H_ */
