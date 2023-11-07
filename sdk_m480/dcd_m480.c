/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Peter Lawrence
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

#include "tusb_option.h"

#if TUSB_OPT_DEVICE_ENABLED && (CFG_TUSB_MCU == OPT_MCU_M484)

#include "device/dcd.h"
#include "amk_hal.h"
#include "amk_printf.h"

/*
 * The DMA functionality of the USBD peripheral does not appear to succeed with
 * transfer lengths that are longer (> 64 bytes) and are not a multiple of 4.
 * Keep disabled for now.
 */
#define USE_DMA     0

/* rather important info unfortunately not provided by device include files */
#define HSUSBD_BUF_SIZE          2048 /* how much USB buffer space there is */
#define HSUSBD_MAX_DMA_LEN     0x1000 /* max bytes that can be DMAed at one time */

enum ep_enum
{
  PERIPH_EPA = 0,
  PERIPH_EPB = 1,
  PERIPH_EPC = 2,
  PERIPH_EPD = 3,
  PERIPH_EPE = 4,
  PERIPH_EPF = 5,
  PERIPH_EPG = 6,
  PERIPH_EPH = 7,
  PERIPH_EPI = 8,
  PERIPH_EPJ = 9,
  PERIPH_EPK = 10,
  PERIPH_EPL = 11,
  PERIPH_MAX_EP,
};

static const uint8_t epcfg_eptype_table[] =
{
  [TUSB_XFER_CONTROL]     = 0, /* won't happen, since control EPs have dedicated registers */
  [TUSB_XFER_ISOCHRONOUS] = 3 << HSUSBD_EPCFG_EPTYPE_Pos,
  [TUSB_XFER_BULK]        = 1 << HSUSBD_EPCFG_EPTYPE_Pos,
  [TUSB_XFER_INTERRUPT]   = 2 << HSUSBD_EPCFG_EPTYPE_Pos,
};

static const uint8_t eprspctl_eptype_table[] =
{
  [TUSB_XFER_CONTROL]     = 0, /* won't happen, since control EPs have dedicated registers */
  [TUSB_XFER_ISOCHRONOUS] = 2 << HSUSBD_EPRSPCTL_MODE_Pos, /* Fly Mode */
  [TUSB_XFER_BULK]        = 0 << HSUSBD_EPRSPCTL_MODE_Pos, /* Auto-Validate Mode */
  [TUSB_XFER_INTERRUPT]   = 1 << HSUSBD_EPRSPCTL_MODE_Pos, /* Manual-Validate Mode */
};

/* set by dcd_set_address() */
static volatile uint8_t assigned_address;

/* reset by bus_reset(), this is used by dcd_edpt_open() to assign USBD peripheral buffer addresses */
static uint32_t bufseg_addr;

/* RAM table needed to track ongoing transfers performed by dcd_edpt_xfer(), dcd_userEP_in_xfer(), and the ISR */
static struct xfer_ctl_t
{
  uint8_t *data_ptr;         /* data_ptr tracks where to next copy data to (for OUT) or from (for IN) */
  // tu_fifo_t* ff; // TODO support dcd_edpt_xfer_fifo API
  union {
    uint16_t in_remaining_bytes; /* for IN endpoints, we track how many bytes are left to transfer */
    uint16_t out_bytes_so_far;   /* but for OUT endpoints, we track how many bytes we've transferred so far */
  };
  uint16_t max_packet_size;  /* needed since device driver only finds out this at runtime */
  uint16_t total_bytes;      /* quantity needed to pass as argument to dcd_event_xfer_complete() (for IN endpoints) */
  uint8_t ep_addr;
  bool dma_requested;
} xfer_table[PERIPH_MAX_EP];

/* in addition to xfer_table, additional bespoke bookkeeping is maintained for control EP0 IN */
static struct
{
  uint8_t *data_ptr;
  uint16_t in_remaining_bytes;
  uint16_t total_bytes;
} ctrl_in_xfer;

static volatile struct xfer_ctl_t *current_dma_xfer;


static void usb_control_send_zlp(void)
{
  HSUSBD->CEPINTSTS = HSUSBD_CEPINTSTS_STSDONEIF_Msk;
  HSUSBD->CEPCTL = 0; /* clear NAKCLR bit */
  HSUSBD->CEPINTEN = HSUSBD_CEPINTEN_STSDONEIEN_Msk;
}

/* map 8-bit ep_addr into peripheral endpoint index (PERIPH_EPA...) */
static HSUSBD_EP_T *ep_entry(uint8_t ep_addr, bool add)
{
  HSUSBD_EP_T *ep;
  enum ep_enum ep_index;
  struct xfer_ctl_t *xfer;

  for (ep_index = PERIPH_EPA, xfer = &xfer_table[PERIPH_EPA], ep = HSUSBD->EP;
       ep_index < PERIPH_MAX_EP;
       ep_index++, xfer++, ep++)
  {
    if (add)
    {
      /* take first peripheral endpoint that is unused */
      if (0 == (ep->EPCFG & HSUSBD_EPCFG_EPEN_Msk)) return ep;
    }
    else
    {
      /* find a peripheral endpoint that matches ep_addr */
      if (xfer->ep_addr == ep_addr) return ep;
    }
  }

  return NULL;
}

/* perform a non-control IN endpoint transfer; this is called by the ISR  */
static void dcd_userEP_in_xfer(struct xfer_ctl_t *xfer, HSUSBD_EP_T *ep)
{
  uint16_t const bytes_now = tu_min16(xfer->in_remaining_bytes, xfer->max_packet_size);

  /* precompute what amount of data will be left */
  xfer->in_remaining_bytes -= bytes_now;

  /*
  if there will be no more data to send, we replace the BUFEMPTYIF EP interrupt with TXPKIF;
  that way, we alert TinyUSB as soon as this last packet has been sent
  */
  if (0 == xfer->in_remaining_bytes)
  {
    ep->EPINTSTS = HSUSBD_EPINTSTS_TXPKIF_Msk;
    ep->EPINTEN = HSUSBD_EPINTEN_TXPKIEN_Msk;
  }

  /* provided buffers are thankfully 32-bit aligned, allowing most data to be transfered as 32-bit */
#if 0 // TODO support dcd_edpt_xfer_fifo API
  if (xfer->ff)
  {
    tu_fifo_read_n_const_addr_full_words(xfer->ff, (void *) (&ep->EPDAT_BYTE), bytes_now);
  }
  else
#endif
  {
    uint16_t countdown = bytes_now;
    while (countdown > 3)
    {
      uint32_t u32;
      memcpy(&u32, xfer->data_ptr, 4);

      ep->EPDAT = u32;
      xfer->data_ptr += 4; countdown -= 4;
    }

    while (countdown--) ep->EPDAT_BYTE = *xfer->data_ptr++;
  }

  /* for short packets, we must nudge the peripheral to say 'that's all folks' */
  if (bytes_now != xfer->max_packet_size) ep->EPRSPCTL = HSUSBD_EPRSPCTL_SHORTTXEN_Msk;
}

/* called by dcd_init() as well as by the ISR during a USB bus reset */
static void bus_reset(void)
{
  for (enum ep_enum ep_index = PERIPH_EPA; ep_index < PERIPH_MAX_EP; ep_index++)
  {
    HSUSBD->EP[ep_index].EPRSPCTL = HSUSBD_EPRSPCTL_FLUSH_Msk;

    HSUSBD->EP[ep_index].EPCFG = 0;
    xfer_table[ep_index].dma_requested = false;
  }

  HSUSBD_ResetDMA();

  /* allocate the default EP0 endpoints */

  HSUSBD_SetEpBufAddr(CEP, 0, CFG_TUD_ENDPOINT0_SIZE);
  HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPTKIEN_Msk|HSUSBD_CEPINTEN_SETUPPKIEN_Msk|HSUSBD_CEPINTEN_STSDONEIEN_Msk);

  //HSUSBD->CEPBUFST = 0;
  //HSUSBD->CEPBUFEND = 0 + CFG_TUD_ENDPOINT0_SIZE - 1UL;

  /* USB RAM beyond what we've allocated above is available to the user */
  bufseg_addr = CFG_TUD_ENDPOINT0_SIZE;
  //HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_STSDONEIEN_Msk | HSUSBD_CEPINTEN_RXPKIEN_Msk | HSUSBD_CEPINTEN_TXPKIEN_Msk);

  /* Reset USB device address */
  HSUSBD_SET_ADDR(0);

  current_dma_xfer = NULL;
}

#if USE_DMA
/* this must only be called by the ISR; it does its best to share the single DMA engine across all user EPs (IN and OUT) */
static void service_dma(void)
{
  if (current_dma_xfer)
    return;

  enum ep_enum ep_index;
  struct xfer_ctl_t *xfer;
  HSUSBD_EP_T *ep;

  for (ep_index = PERIPH_EPA, xfer = &xfer_table[PERIPH_EPA], ep = &HSUSBD->EP[PERIPH_EPA]; ep_index < PERIPH_MAX_EP; ep_index++, xfer++, ep++)
  {
    uint16_t const available_bytes = ep->EPDATCNT & HSUSBD_EPDATCNT_DATCNT_Msk;

    if (!xfer->dma_requested || !available_bytes)
      continue;

    /*
    instruct DMA to copy the data from the PC to the previously provided buffer
    when the bus interrupt DMADONEIEN subsequently fires, the transfer will have finished
    */
    HSUSBD->DMACTL = xfer->ep_addr & HSUSBD_DMACTL_EPNUM_Msk;
    HSUSBD->DMAADDR = (uint32_t)xfer->data_ptr;
    HSUSBD->DMACNT = available_bytes;
    HSUSBD->BUSINTSTS = HSUSBD_BUSINTSTS_DMADONEIF_Msk;
    xfer->out_bytes_so_far += available_bytes;
    current_dma_xfer = xfer;
    HSUSBD->DMACTL |= HSUSBD_DMACTL_DMAEN_Msk;

    return;
  }
}
#endif

/* centralized location for USBD interrupt enable bit masks */
/*
static const uint32_t enabled_irqs = HSUSBD_GINTEN_USBIEN_Msk | \
  HSUSBD_GINTEN_EPAIEN_Msk | HSUSBD_GINTEN_EPBIEN_Msk | HSUSBD_GINTEN_EPCIEN_Msk | HSUSBD_GINTEN_EPDIEN_Msk | HSUSBD_GINTEN_EPEIEN_Msk | HSUSBD_GINTEN_EPFIEN_Msk | \
  HSUSBD_GINTEN_EPGIEN_Msk | HSUSBD_GINTEN_EPHIEN_Msk | HSUSBD_GINTEN_EPIIEN_Msk | HSUSBD_GINTEN_EPJIEN_Msk | HSUSBD_GINTEN_EPKIEN_Msk | HSUSBD_GINTEN_EPLIEN_Msk | \
  HSUSBD_GINTEN_CEPIEN_Msk;
  */

/*
  M484 TinyUSB API driver implementation
*/

void dcd_init(uint8_t rhport)
{
  (void) rhport;

  /* Initial USB engine */
  HSUSBD_ENABLE_PHY();

  /* wait PHY clock ready */

  while (1) {
    HSUSBD->EP[EPA].EPMPS = 0x20ul;
    if (HSUSBD->EP[EPA].EPMPS == 0x20ul) {
        HSUSBD->EP[EPA].EPMPS = 0x0ul;
        break;
    }
  }

  //uint32_t volatile i;
  //for (i = 0; i < 0x10000; i++);

  HSUSBD->OPER &= ~HSUSBD_OPER_HISPDEN_Msk;   /* full-speed */

  bus_reset();

  /* Enable USB BUS, CEP global interrupt */
  HSUSBD_ENABLE_USB_INT(HSUSBD_GINTEN_USBIEN_Msk | HSUSBD_GINTEN_CEPIEN_Msk);

  /* Enable BUS interrupt */
  HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RESUMEIEN_Msk | HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_VBUSDETIEN_Msk | HSUSBD_BUSINTEN_SOFIEN_Msk);

  dcd_int_enable(0);

  HSUSBD->OPER = HSUSBD_OPER_HISPDEN_Msk;   /* high-speed */
  dcd_connect(0);
}

void dcd_int_enable(uint8_t rhport)
{
  (void) rhport;
  NVIC_EnableIRQ(USBD20_IRQn);
}

void dcd_int_disable(uint8_t rhport)
{
  (void) rhport;
  NVIC_DisableIRQ(USBD20_IRQn);
}

void dcd_set_address(uint8_t rhport, uint8_t dev_addr)
{
  (void) rhport;
  usb_control_send_zlp(); /* SET_ADDRESS is the one exception where TinyUSB doesn't use dcd_edpt_xfer() to generate a ZLP */
  assigned_address = dev_addr;
}

void dcd_remote_wakeup(uint8_t rhport)
{
  (void) rhport;
  HSUSBD->OPER |= HSUSBD_OPER_RESUMEEN_Msk;
}

bool dcd_edpt_open(uint8_t rhport, tusb_desc_endpoint_t const * p_endpoint_desc)
{
  (void) rhport;

  HSUSBD_EP_T *ep = ep_entry(p_endpoint_desc->bEndpointAddress, true);
  TU_ASSERT(ep);

  /* mine the data for the information we need */
  int const dir = tu_edpt_dir(p_endpoint_desc->bEndpointAddress);
  int const size = tu_edpt_packet_size(p_endpoint_desc);
  tusb_xfer_type_t const type = p_endpoint_desc->bmAttributes.xfer;
  struct xfer_ctl_t *xfer = &xfer_table[ep - HSUSBD->EP];

  /* allocate buffer from USB RAM */
  ep->EPBUFST = bufseg_addr;
  bufseg_addr += size;
  ep->EPBUFEND = bufseg_addr - 1;
  TU_ASSERT(bufseg_addr <= HSUSBD_BUF_SIZE);

  ep->EPMPS = size;

  ep->EPRSPCTL = HSUSBD_EP_RSPCTL_FLUSH | eprspctl_eptype_table[type];

  /* construct USB Configuration Register value and then write it */
  uint32_t num = tu_edpt_number(p_endpoint_desc->bEndpointAddress);
  uint32_t cfg = num << HSUSBD_EPCFG_EPNUM_Pos;
  if (TUSB_DIR_IN == dir)
    cfg |= HSUSBD_EPCFG_EPDIR_Msk;
  cfg |= epcfg_eptype_table[type] | HSUSBD_EPCFG_EPEN_Msk;
  ep->EPCFG = cfg;

  /* make a note of the endpoint particulars */
  xfer->max_packet_size = size;
  xfer->ep_addr = p_endpoint_desc->bEndpointAddress;


  /* enable endpoint interrupt */
  HSUSBD->GINTEN |= (0x1UL << (HSUSBD_GINTEN_EPAIEN_Pos + (num - PERIPH_EPA)));

  if (dir == TUSB_DIR_OUT) {
    HSUSBD_ENABLE_EP_INT(num, HSUSBD_EPINTEN_RXPKIEN_Msk | HSUSBD_EPINTEN_SHORTRXIEN_Msk | HSUSBD_EPINTEN_BUFFULLIEN_Msk);
  } else {
    HSUSBD_ENABLE_EP_INT(num, HSUSBD_EPINTEN_TXPKIEN_Msk);
  }

  return true;
}

void dcd_edpt_close (uint8_t rhport, uint8_t ep_addr)
{
  uint8_t num = tu_edpt_number(ep_addr);
  HSUSBD->EP[num].EPRSPCTL = (HSUSBD->EP[num].EPRSPCTL & HSUSBD_EP_RSPCTL_MODE_MASK) | HSUSBD_EPRSPCTL_FLUSH_Msk;

  HSUSBD->GINTEN &= ~(0x1UL << (HSUSBD_GINTEN_EPAIEN_Pos + num - 1));
  HSUSBD_ENABLE_EP_INT(num, 0);
}

void dcd_edpt_close_all (uint8_t rhport)
{
  (void) rhport;
  // TODO implement dcd_edpt_close_all()
}

bool dcd_edpt_xfer(uint8_t rhport, uint8_t ep_addr, uint8_t *buffer, uint16_t total_bytes)
{
  (void) rhport;

  if (0x80 == ep_addr) /* control EP0 IN */
  {
    if (total_bytes)
    {
      HSUSBD->CEPCTL = HSUSBD_CEPCTL_FLUSH_Msk;
      ctrl_in_xfer.data_ptr = buffer;
      ctrl_in_xfer.in_remaining_bytes = total_bytes;
      ctrl_in_xfer.total_bytes = total_bytes;
      HSUSBD->CEPINTSTS = HSUSBD_CEPINTSTS_INTKIF_Msk;
      HSUSBD->CEPINTEN = HSUSBD_CEPINTEN_INTKIEN_Msk;
    }
    else
    {
      usb_control_send_zlp();
    }
  }
  else if (0x00 == ep_addr) /* control EP0 OUT */
  {
    if (total_bytes)
    {
      /* if TinyUSB is asking for EP0 OUT data, it is almost certainly already in the buffer */
      while (total_bytes < HSUSBD->CEPRXCNT);
      for (int count = 0; count < total_bytes; count++)
        *buffer++ = HSUSBD->CEPDAT_BYTE;
      
      dcd_event_xfer_complete(0, ep_addr, total_bytes, XFER_RESULT_SUCCESS, true);
    }
  }
  else
  {
    /* mine the data for the information we need */
    tusb_dir_t dir = tu_edpt_dir(ep_addr);
    HSUSBD_EP_T *ep = ep_entry(ep_addr, false);
    TU_ASSERT(ep);
    struct xfer_ctl_t *xfer = &xfer_table[ep - HSUSBD->EP];

    /* store away the information we'll needing now and later */
    xfer->data_ptr = buffer;
    // xfer->ff       = NULL; // TODO support dcd_edpt_xfer_fifo API
    xfer->in_remaining_bytes = total_bytes;
    xfer->total_bytes = total_bytes;

    if (TUSB_DIR_IN == dir)
    {
      ep->EPINTEN = HSUSBD_EPINTEN_BUFEMPTYIEN_Msk;
    }
    else
    {
      xfer->out_bytes_so_far = 0;
      ep->EPINTEN = HSUSBD_EPINTEN_RXPKIEN_Msk;
    }
  }

  return true;
}

#if 0 // TODO support dcd_edpt_xfer_fifo API
bool dcd_edpt_xfer_fifo (uint8_t rhport, uint8_t ep_addr, tu_fifo_t * ff, uint16_t total_bytes)
{
  (void) rhport;

  TU_ASSERT(0x80 != ep_addr && 0x00 != ep_addr);  // Must not be used for control stuff

  /* mine the data for the information we need */
  tusb_dir_t dir = tu_edpt_dir(ep_addr);
  HSUSBD_EP_T *ep = ep_entry(ep_addr, false);
  struct xfer_ctl_t *xfer = &xfer_table[ep - HSUSBD->EP];

  /* store away the information we'll needing now and later */
  xfer->data_ptr = NULL;      // Indicates a FIFO shall be used
  xfer->ff       = ff;
  xfer->in_remaining_bytes = total_bytes;
  xfer->total_bytes = total_bytes;

  if (TUSB_DIR_IN == dir)
  {
    ep->EPINTEN = HSUSBD_EPINTEN_BUFEMPTYIEN_Msk;
  }
  else
  {
    xfer->out_bytes_so_far = 0;
    ep->EPINTEN = HSUSBD_EPINTEN_RXPKIEN_Msk;
  }

  return true;
}
#endif

void dcd_edpt_stall(uint8_t rhport, uint8_t ep_addr)
{
  (void) rhport;

  if (tu_edpt_number(ep_addr))
  {
    HSUSBD_EP_T *ep = ep_entry(ep_addr, false);
    TU_ASSERT(ep, );
    ep->EPRSPCTL = (ep->EPRSPCTL & 0xf7) | HSUSBD_EPRSPCTL_HALT_Msk;
  }
  else
  {
    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
  }
}

void dcd_edpt_clear_stall(uint8_t rhport, uint8_t ep_addr)
{
  (void) rhport;

  if (tu_edpt_number(ep_addr))
  {
    HSUSBD_EP_T *ep = ep_entry(ep_addr, false);
    TU_ASSERT(ep, );
    ep->EPRSPCTL = (ep->EPRSPCTL & HSUSBD_EP_RSPCTL_MODE_MASK) | HSUSBD_EPRSPCTL_TOGGLE_Msk;
  } 
  else 
  {
    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR_Msk);
  }
}
void dcd_int_handler(uint8_t rhport)
{
  (void) rhport;

  uint32_t status = HSUSBD->GINTSTS;
  amk_printf("Global int status: 0x%x\n", status);

  /* USB interrupt */
  if (status & HSUSBD_GINTSTS_USBIF_Msk)
  {
    uint32_t bus_state = HSUSBD->BUSINTSTS;
    amk_printf("BUS int status: 0x%x\n", bus_state);

    if (bus_state & HSUSBD_BUSINTSTS_SOFIF_Msk)
    {
      /* Start-Of-Frame event */
      dcd_event_bus_signal(0, DCD_EVENT_SOF, true);
    }

    if (bus_state & HSUSBD_BUSINTSTS_RSTIF_Msk)
    {
      bus_reset();

      HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_RESUMEIEN_Msk | HSUSBD_BUSINTEN_SUSPENDIEN_Msk | HSUSBD_BUSINTEN_SOFIEN_Msk);
      HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk);

      HSUSBD_CLR_CEP_INT_FLAG(0x1FFC);

      tusb_speed_t speed = (HSUSBD->OPER & HSUSBD_OPER_CURSPD_Msk) ? TUSB_SPEED_HIGH : TUSB_SPEED_FULL;
      dcd_event_bus_reset(0, speed, true);
      amk_printf("BUS RST int status: 0x%x\n", bus_state);
    }

    if (bus_state & HSUSBD_BUSINTSTS_RESUMEIF_Msk)
    {
      HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_SUSPENDIEN_Msk | HSUSBD_BUSINTEN_SOFIEN_Msk);
      dcd_event_bus_signal(0, DCD_EVENT_RESUME, true);
    }

    if (bus_state & HSUSBD_BUSINTSTS_SUSPENDIF_Msk)
    {
      HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_RESUMEIEN_Msk | HSUSBD_BUSINTEN_SOFIEN_Msk);
      dcd_event_bus_signal(0, DCD_EVENT_SUSPEND, true);
    }

    if (bus_state & HSUSBD_BUSINTSTS_HISPDIF_Msk)
    {
      HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk);
    }

    if (bus_state & HSUSBD_BUSINTSTS_DMADONEIF_Msk)
    {
      amk_printf("HSUSB DMA done\n");
#if USE_DMA
      if (current_dma_xfer)
      {
        current_dma_xfer->dma_requested = false;

        uint16_t available_bytes = HSUSBD->DMACNT & HSUSBD_DMACNT_DMACNT_Msk;

        /* if the most recent DMA finishes the transfer, alert TinyUSB; otherwise, the next RXPKIF/INTKIF endpoint interrupt will prompt the next DMA */
        if ( (current_dma_xfer->total_bytes == current_dma_xfer->out_bytes_so_far) || (available_bytes < current_dma_xfer->max_packet_size) )
        {
          dcd_event_xfer_complete(0, current_dma_xfer->ep_addr, current_dma_xfer->out_bytes_so_far, XFER_RESULT_SUCCESS, true);
        }

        current_dma_xfer = NULL;
        service_dma();
      }
#endif
    }

    if (bus_state & HSUSBD_BUSINTSTS_VBUSDETIF_Msk)
    {
      if (HSUSBD_IS_ATTACHED()) {
        amk_printf("HSUSB enabled USB\n");
        HSUSBD_ENABLE_USB();
      } else {
        HSUSBD_DISABLE_USB();
        amk_printf("HSUSB disabled USB\n");
      }
    }

    HSUSBD->BUSINTSTS = bus_state & (HSUSBD_BUSINTSTS_SOFIF_Msk | HSUSBD_BUSINTSTS_RSTIF_Msk | HSUSBD_BUSINTSTS_RESUMEIF_Msk | HSUSBD_BUSINTSTS_SUSPENDIF_Msk | HSUSBD_BUSINTSTS_HISPDIF_Msk | HSUSBD_BUSINTSTS_DMADONEIF_Msk | HSUSBD_BUSINTSTS_PHYCLKVLDIF_Msk | HSUSBD_BUSINTSTS_VBUSDETIF_Msk);
  }

  if (status & HSUSBD_GINTSTS_CEPIF_Msk)
  {
    uint32_t cep_state = HSUSBD->CEPINTSTS & HSUSBD->CEPINTEN;
    amk_printf("CEP int status: 0x%x\n", cep_state);

    if (cep_state & HSUSBD_CEPINTSTS_SETUPTKIF_Msk)
    {
      HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_SETUPTKIF_Msk);
    }
    else if (cep_state & HSUSBD_CEPINTSTS_SETUPPKIF_Msk)
    {
      amk_printf("HSUSBD setup packet\n");
      HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_SETUPPKIF_Msk);

      /* get SETUP packet from USB buffer */
      uint8_t setup_packet[8];
      setup_packet[0] = (uint8_t)(HSUSBD->SETUP1_0 >> 0);
      setup_packet[1] = (uint8_t)(HSUSBD->SETUP1_0 >> 8);
      setup_packet[2] = (uint8_t)(HSUSBD->SETUP3_2 >> 0);
      setup_packet[3] = (uint8_t)(HSUSBD->SETUP3_2 >> 8);
      setup_packet[4] = (uint8_t)(HSUSBD->SETUP5_4 >> 0);
      setup_packet[5] = (uint8_t)(HSUSBD->SETUP5_4 >> 8);
      setup_packet[6] = (uint8_t)(HSUSBD->SETUP7_6 >> 0);
      setup_packet[7] = (uint8_t)(HSUSBD->SETUP7_6 >> 8);
      dcd_event_setup_received(0, setup_packet, true);
    }
    else if (cep_state & HSUSBD_CEPINTSTS_OUTTKIF_Msk)
    {
        HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_OUTTKIF_Msk);
        HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_STSDONEIEN_Msk);
    }
    else if (cep_state & HSUSBD_CEPINTSTS_INTKIF_Msk)
    {
        HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
        if (!(cep_state & HSUSBD_CEPINTSTS_STSDONEIF_Msk))
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_TXPKIF_Msk);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_TXPKIEN_Msk);
            uint16_t bytes_now = tu_min16(ctrl_in_xfer.in_remaining_bytes, CFG_TUD_ENDPOINT0_SIZE);
            for (int count = 0; count < bytes_now; count++)
              HSUSBD->CEPDAT_BYTE = *ctrl_in_xfer.data_ptr++;
            ctrl_in_xfer.in_remaining_bytes -= bytes_now;
            HSUSBD_START_CEP_IN(bytes_now);
        }
        else
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_TXPKIF_Msk);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_TXPKIEN_Msk|HSUSBD_CEPINTEN_STSDONEIEN_Msk);
        }
    }
    else if (cep_state & HSUSBD_CEPINTSTS_PINGIF_Msk)
    {
      HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_PINGIF_Msk);
    }
    else if (cep_state & HSUSBD_CEPINTSTS_TXPKIF_Msk)
    {
        HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
        HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
        /* alert TinyUSB that the EP0 IN transfer has finished */
        if ( (0 == ctrl_in_xfer.in_remaining_bytes) || (0 == ctrl_in_xfer.total_bytes) )
          dcd_event_xfer_complete(0, 0x80, ctrl_in_xfer.total_bytes, XFER_RESULT_SUCCESS, true);

        if (ctrl_in_xfer.in_remaining_bytes)
        {
          HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
          HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);
        }
        else
        {
          /* TinyUSB does its own fragmentation and ZLP for EP0; a transfer of zero means a ZLP */
          if (0 == ctrl_in_xfer.total_bytes)
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_ZEROLEN);

          HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
          HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk|HSUSBD_CEPINTEN_STSDONEIEN_Msk);
        }

        HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_TXPKIF_Msk);
    }
    else if (cep_state & HSUSBD_CEPINTSTS_RXPKIF_Msk)
    {
      HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_RXPKIF_Msk);
      HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
      HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk|HSUSBD_CEPINTEN_STSDONEIEN_Msk);
    }
    else if (cep_state & HSUSBD_CEPINTSTS_NAKIF_Msk)
    {
      HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_NAKIF_Msk);
    }
    else if (cep_state & HSUSBD_CEPINTSTS_STALLIF_Msk)
    {
      HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STALLIF_Msk);
    }
    else if (cep_state & HSUSBD_CEPINTSTS_ERRIF_Msk)
    {
      HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_ERRIF_Msk);
    }
    else if (cep_state & HSUSBD_CEPINTSTS_STSDONEIF_Msk)
    {
      if((HSUSBD->FADDR != assigned_address) && (HSUSBD->FADDR == 0))
      {
        HSUSBD->FADDR = assigned_address;

        for (enum ep_enum ep_index = PERIPH_EPA; ep_index < PERIPH_MAX_EP; ep_index++)
        {
          if (HSUSBD->EP[ep_index].EPCFG & HSUSBD_EPCFG_EPEN_Msk) HSUSBD->EP[ep_index].EPRSPCTL = HSUSBD_EPRSPCTL_TOGGLE_Msk;
        }
      }
      HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
      HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk);
    }
    else if (cep_state & HSUSBD_CEPINTSTS_BUFFULLIF_Msk)
    {
      HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_BUFFULLIF_Msk);
    }
    else if (cep_state & HSUSBD_CEPINTSTS_BUFEMPTYIF_Msk)
    {
      HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_BUFEMPTYIF_Msk);
    }

    return;
  }

  if (status & (HSUSBD_GINTSTS_EPAIF_Msk | HSUSBD_GINTSTS_EPBIF_Msk | HSUSBD_GINTSTS_EPCIF_Msk | HSUSBD_GINTSTS_EPDIF_Msk \
          | HSUSBD_GINTSTS_EPEIF_Msk | HSUSBD_GINTSTS_EPFIF_Msk | HSUSBD_GINTSTS_EPGIF_Msk | HSUSBD_GINTSTS_EPHIF_Msk \
          | HSUSBD_GINTSTS_EPIIF_Msk | HSUSBD_GINTSTS_EPJIF_Msk | HSUSBD_GINTSTS_EPKIF_Msk | HSUSBD_GINTSTS_EPLIF_Msk))
  {
    /* service PERIPH_EPA through PERIPH_EPL */
    enum ep_enum ep_index;
    uint32_t mask;
    struct xfer_ctl_t *xfer;
    HSUSBD_EP_T *ep;
    for (ep_index = PERIPH_EPA, mask = HSUSBD_GINTSTS_EPAIF_Msk, xfer = &xfer_table[PERIPH_EPA], ep = &HSUSBD->EP[PERIPH_EPA]; ep_index < PERIPH_MAX_EP; ep_index++, mask <<= 1, xfer++, ep++)
    {
      if(status & mask)
      {
        uint8_t const ep_addr = xfer->ep_addr;
        bool const out_ep = !(ep_addr & TUSB_DIR_IN_MASK);
        uint32_t ep_state = ep->EPINTSTS & ep->EPINTEN;

        if (out_ep)
        {
#if USE_DMA
          xfer->dma_requested = true;
          service_dma();
#else
          uint16_t const available_bytes = ep->EPDATCNT & HSUSBD_EPDATCNT_DATCNT_Msk;
          /* copy the data from the PC to the previously provided buffer */
#if 0 // TODO support dcd_edpt_xfer_fifo API
          if (xfer->ff)
          {
            tu_fifo_write_n_const_addr_full_words(xfer->ff, (const void *) &ep->EPDAT_BYTE, tu_min16(available_bytes, xfer->total_bytes - xfer->out_bytes_so_far));
          }
          else
#endif
          {
            for (int count = 0; (count < available_bytes) && (xfer->out_bytes_so_far < xfer->total_bytes); count++, xfer->out_bytes_so_far++)
            {
              *xfer->data_ptr++ = ep->EPDAT_BYTE;
            }
          }

          /* when the transfer is finished, alert TinyUSB; otherwise, continue accepting more data */
          if ( (xfer->total_bytes == xfer->out_bytes_so_far) || (available_bytes < xfer->max_packet_size) )
          {
            dcd_event_xfer_complete(0, ep_addr, xfer->out_bytes_so_far, XFER_RESULT_SUCCESS, true);
          }
#endif

        }
        else if (ep_state & HSUSBD_EPINTSTS_BUFEMPTYIF_Msk)
        {
          /* send any remaining data */
          dcd_userEP_in_xfer(xfer, ep);
        }
        else if (ep_state & HSUSBD_EPINTSTS_TXPKIF_Msk)
        {
          /* alert TinyUSB that we've finished */
          dcd_event_xfer_complete(0, ep_addr, xfer->total_bytes, XFER_RESULT_SUCCESS, true);
          ep->EPINTEN = 0;
        }

        ep->EPINTSTS = ep_state;
      }
    }
  }
}

void dcd_disconnect(uint8_t rhport)
{
  (void) rhport;
  HSUSBD_SET_SE0();
}

void dcd_connect(uint8_t rhport)
{
  (void) rhport;
  HSUSBD_CLR_SE0();
}

#endif
