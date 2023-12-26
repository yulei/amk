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

#ifndef _DWC2_APM32_H_
#define _DWC2_APM32_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"

/*
#define USB_OTG_HS_BASE         ((uint32_t)0x40040000)
#define USB_OTG_HS_HOST_BASE    ((uint32_t)USB_OTG_HS_BASE + 0x400)
#define USB_OTG_HS_DEV_BASE     ((uint32_t)USB_OTG_HS_BASE + 0x800)
#define USB_OTG_HS_FIFO_BASE    ((uint32_t)USB_OTG_HS_BASE + 0x1000)
#define USB_OTG_HS_PWR_BASE     ((uint32_t)USB_OTG_HS_BASE + 0xE00)

#define USB_OTG_HS              ((USB_OTG_GLOBAL_T *)USB_OTG_HS_BASE)
#define USB_OTG_HS_H            ((USB_OTG_HOST_T *)USB_OTG_HS_HOST_BASE)
#define USB_OTG_HS_D            ((USB_OTG_DEVICE_T *)USB_OTG_HS_DEV_BASE)

#define USB_OTG_HS_FIFO         ((USB_OTG_FIFO_T *) USB_OTG_HS_FIFO_BASE)
#define USB_OTG_FIFO_OFFSET     0x1000
#define USB_OTG_HS_PWR          ((USB_OTG_PWRCLK_T *) USB_OTG_HS_PWR_BASE)
#define USB_OTG_HS2_BASE        ((uint32_t)USB_OTG_HS_BASE + 0x200)
#define USB_OTG_HS2             ((USB_OTG_HS2_T *) USB_OTG_HS2_BASE)
*/

// EP_MAX       : Max number of bi-directional endpoints including EP0
// EP_FIFO_SIZE : Size of dedicated USB SRAM
#define EP_MAX_HS       8
#define EP_FIFO_SIZE_HS 4096
#define USB_OTG_HS_PERIPH_BASE  0x40040000UL
#define USB_OTG_HS_IRQn 77
#define DWC2_EP_MAX   EP_MAX_HS

#ifndef __IOM
#define __IOM volatile
#endif

/**
  * @brief USB on the go high speed (OTG_HS2)
  */

typedef struct
{
    union {
        __IOM uint32_t USB_SWITCH;
        struct {
            __IOM uint32_t usb_switch : 1;
        } USB_SWITCH_B;
    } ;

    union {
        __IOM uint32_t POWERON_CORE;
        struct {
            __IOM uint32_t poweron_core : 1;
        } POWERON_CORE_B;
    } ;

    union {
        __IOM uint32_t USB_PLL_EN;
        struct {
            __IOM uint32_t usb_pll_en : 1;
        } USB_PLL_EN_B;
    } ;

    union {
        __IOM uint32_t SHORT_5V_ENABLE;
        struct {
            __IOM uint32_t short_5v_enable : 1;
        } SHORT_5V_ENABLE_B;
    } ;

    union {
        __IOM uint32_t OTG_SUSPENDM;
        struct {
            __IOM uint32_t otg_suspendm : 1;
        } OTG_SUSPENDM_B;
    } ;

    union {
        __IOM uint32_t TXBITSTUFFENABLE;
        struct {
            __IOM uint32_t txbitstuffenable : 1;
        } TXBITSTUFFENABLE_B;
    } ;

    union {
        __IOM uint32_t PLLICP_SEL_I2C;
        struct {
            __IOM uint32_t pllicp_sel_i2c : 1;
        } PLLICP_SEL_I2C_B;
    } ;

    union {
        __IOM  uint32_t HSZR_CNTL_I2C;
        struct {
            __IOM  uint32_t hszr_cntl_i2c : 3;
        } HSZR_CNTL_I2C_B;
    } ;

    union {
        __IOM  uint32_t SQVTH_CNTL_I2C;
        struct {
            __IOM  uint32_t sqvth_cntl_i2c : 2;
        } SQVTH_CNTL_I2C_B;
    } ;

    union {
        __IOM  uint32_t SW_RREF_I2C;
        struct {
            __IOM  uint32_t sw_rref_i2c : 3;
        } SW_RREF_I2C_B;
    } ;

    union {
        __IOM  uint32_t SW_BUF_I2C;
        struct {
            __IOM  uint32_t sw_buf_i2c : 1;
        } SW_BUF_I2C_B;
    } ;

    union {
        __IOM uint32_t TX2RX_T_CFG_I2C;
        struct {
            __IOM uint32_t tx2rx_t_cfg_i2c : 2;
        } TX2RX_T_CFG_I2C_B;
    } ;

    union {
        __IOM uint32_t TEST_ANA_FAST_I2C;
        struct {
            __IOM uint32_t test_ana_fast_i2c : 3;
        } TEST_ANA_FAST_I2C_B;
    } ;

    union {
        __IOM uint32_t CLK_MODE_I2C;
        struct {
            __IOM uint32_t clk_mode_i2c : 3;
        } CLK_MODE_I2C_B;
    } ;

    union {
        __IOM  uint32_t USB_DBNCE_FLTR_BYPASS;
        struct {
            __IOM  uint32_t usb_dbnce_fltr_bypass : 1;
        } USB_DBNCE_FLTR_BYPASS_B;
    } ;

    union {
        __IOM uint32_t USB_SS_SCALEDOWN_MODE;
        struct {
            __IOM uint32_t usb_ss_scaledown_mode : 2;
        } USB_SS_SCALEDOWN_MODE_B;
    } ;
} USB_OTG_HS2_T;

#define USB_OTG_HS2_BASE        ((uint32_t)USB_OTG_HS_PERIPH_BASE+ 0x200)
#define USB_OTG_HS2             ((USB_OTG_HS2_T *) USB_OTG_HS2_BASE)


// On STM32 for consistency we associate
// - Port0 to OTG_FS, and Port1 to OTG_HS
static const dwc2_controller_t _dwc2_controller[] =
{
  { .reg_base = USB_OTG_HS_PERIPH_BASE, .irqnum = USB_OTG_HS_IRQn, .ep_count = EP_MAX_HS, .ep_fifo_size = EP_FIFO_SIZE_HS},
};

//--------------------------------------------------------------------+
//
//--------------------------------------------------------------------+

// SystemCoreClock is already included by family header
// extern uint32_t SystemCoreClock;

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
static inline void dwc2_remote_wakeup_delay(void)
{
  // try to delay for 1 ms
  uint32_t count = SystemCoreClock / 1000;
  while ( count-- ) __NOP();
}

// MCU specific PHY init, called BEFORE core reset
static inline void dwc2_phy_init(dwc2_regs_t * dwc2, uint8_t hs_phy_type)
{
  // enable showd
  USB_OTG_HS2->USB_SWITCH_B.usb_switch = 1;
  USB_OTG_HS2->POWERON_CORE_B.poweron_core = 1;
  USB_OTG_HS2->OTG_SUSPENDM_B.otg_suspendm = 1;
  USB_OTG_HS2->SW_RREF_I2C_B.sw_rref_i2c = 0x05;

}

// MCU specific PHY update, it is called AFTER init() and core reset
static inline void dwc2_phy_update(dwc2_regs_t * dwc2, uint8_t hs_phy_type)
{
  dwc2->gusbcfg = (dwc2->gusbcfg & ~GUSBCFG_TRDT_Msk) | (0x9u << GUSBCFG_TRDT_Pos);
}

#ifdef __cplusplus
}
#endif

#endif /* _DWC2_STM32_H_ */
