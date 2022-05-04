/**
 * @file dcd_gd32.c
 * @author astro
 * @brief 
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "tusb_option.h"

#ifndef CFG_TUD_ENABLED
#error "use Device driver without enable tinyusb devide stack"
#endif

#include "gd32e10x.h"
#include "gd32_util.h"
#include "drv_usb_core.h"
#include "drv_usb_dev.h"
#include "drv_usbd_int.h"
#include "osal/osal.h"
#include "generic_hal.h"
#include "device/dcd.h"

#include "amk_printf.h"

#ifndef DCD_DEBUG
#define DCD_DEBUG 1
#endif

#if DCD_DEBUG
#define dcd_debug  amk_printf
#else
#define dcd_debug(...)
#endif

usb_core_driver usbd_core;

static uint8_t usbd_bus_reset(void *udev);
static uint8_t usbd_unplugged(void *udev);
static uint8_t usbd_suspend(void *udev);
static uint8_t usbd_resume(void *udev);
static uint8_t usbd_sof(void *udev);
static uint8_t usbd_setup_received(void *udev);
static uint8_t usbd_xfer_complete(void *udev, uint32_t ep_addr);

usbd_int_cb usbd_int_op = {
    .bus_reset = usbd_bus_reset,
    .unplugged = usbd_unplugged,
    .suspend = usbd_suspend,
    .resume = usbd_resume,
    .sof = usbd_sof,
    .setup_received = usbd_setup_received,
    .xfer_complete = usbd_xfer_complete,
};

usbd_int_cb *usbd_int_fop = &usbd_int_op;

#if 1
static void dcd_open_ep0(usb_core_driver *udev)
{
    // open ep0 out
    udev->dev.transc_out[0] = (usb_transc) {
        .ep_addr = {
            .dir = 0U
        },

        .ep_type = USB_EPTYPE_CTRL,
        .max_len = USB_FS_EP0_MAX_LEN
    };
    (void)usb_transc_active (udev, &udev->dev.transc_out[0]);

    // open ep0 in
    udev->dev.transc_in[0] = (usb_transc) {
        .ep_addr = {
            .dir = 1U
        },

        .ep_type = USB_EPTYPE_CTRL,
        .max_len = USB_FS_EP0_MAX_LEN
    };
    (void)usb_transc_active (udev, &udev->dev.transc_in[0]);

    amk_printf("GD32 ep0 opened\n");
}
#endif

void dcd_init(uint8_t rhport)
{
    /* create serial string */
    //serial_string_get(udev->dev.desc->strings[STR_IDX_SERIAL]);
    usb_core_driver *udev = &usbd_core;

    usb_basic_init (&udev->bp, &udev->regs, USB_CORE_ENUM_FS);

    usb_globalint_disable(&udev->regs);

    usb_core_init (udev->bp, &udev->regs);

    dcd_disconnect(0);

    usb_curmode_set(&udev->regs, DEVICE_MODE);

    usb_devcore_init(udev);

    usb_globalint_enable(&udev->regs);

    dcd_connect(0);
    dcd_debug("dcd_init\n");
}

void dcd_int_handler(uint8_t rhport)
{
    usbd_isr(&usbd_core);
}

void dcd_int_enable (uint8_t rhport)
{
    usb_core_driver *udev = &usbd_core;
    usb_globalint_enable(&udev->regs);
}

void dcd_int_disable(uint8_t rhport)
{
    usb_core_driver *udev = &usbd_core;
    usb_globalint_disable(&udev->regs);
}

void dcd_set_address(uint8_t rhport, uint8_t dev_addr)
{
    usb_core_driver *udev = &usbd_core;
    usb_devaddr_set(udev, dev_addr);
    dcd_edpt_xfer(rhport, 0x80, NULL, 0);
    dcd_debug("dcd_set_address\n");
}

void dcd_remote_wakeup(uint8_t rhport)
{
    usb_core_driver *udev = &usbd_core;
    usb_rwkup_set(udev);
    usb_delay_ms(5);
    usb_rwkup_reset(udev);
}

void dcd_connect(uint8_t rhport)
{
    usb_core_driver *udev = &usbd_core;
    usb_dev_connect (udev);
    usb_delay_ms(3U);
}

void dcd_disconnect(uint8_t rhport)
{
    usb_core_driver *udev = &usbd_core;
    usb_dev_disconnect (udev);
    usb_delay_ms(3U);
}

// Invoked when a control transfer's status stage is complete.
// May help DCD to prepare for next control transfer, this API is optional.
// void dcd_edpt0_status_complete(uint8_t rhport, tusb_control_request_t const * request) TU_ATTR_WEAK;

bool dcd_edpt_open(uint8_t rhport, tusb_desc_endpoint_t const * p_ep)
{
    usb_core_driver *udev = &usbd_core;
    usb_transc *transc = NULL;

    uint8_t ep_num = tu_edpt_number(p_ep->bEndpointAddress);
    uint8_t ep_dir = tu_edpt_dir(p_ep->bEndpointAddress);

    /* set endpoint direction */
    if (ep_dir) {
        transc = &udev->dev.transc_in[ep_num];
        transc->ep_addr.dir = 1U;
    } else {
        transc = &udev->dev.transc_out[ep_num];
        transc->ep_addr.dir = 0U;
    }

    transc->ep_addr.num = ep_num;
    transc->max_len = p_ep->wMaxPacketSize;
    transc->ep_type = p_ep->bmAttributes.xfer;

    usb_transc_active(udev, transc);

    dcd_debug("dcd_edpt_open\n");
    return true;
}

void dcd_edpt_close(uint8_t rhport, uint8_t ep_addr)
{
    usb_core_driver *udev = &usbd_core;
    usb_transc *transc = NULL;

    uint8_t ep_num = tu_edpt_number(ep_addr);
    uint8_t ep_dir = tu_edpt_dir(ep_addr);

    if (ep_dir) {
        transc = &udev->dev.transc_in[ep_num];
    } else {
        transc = &udev->dev.transc_out[ep_num];
    }

    usb_transc_deactivate (udev, transc);
}

void dcd_edpt_close_all (uint8_t rhport)
{}

bool dcd_edpt_xfer(uint8_t rhport, uint8_t ep_addr, uint8_t * buffer, uint16_t total_bytes)
{
    usb_core_driver *udev = &usbd_core;
    uint8_t const ep_num = tu_edpt_number(ep_addr);
    uint8_t const ep_dir = tu_edpt_dir(ep_addr);

    if (ep_dir == TUSB_DIR_IN) {
        usb_transc *transc = &udev->dev.transc_in[ep_num];

        transc->xfer_buf = buffer;
        transc->xfer_len = total_bytes;
        transc->xfer_count = 0U;

        if ((uint8_t)USB_USE_DMA == udev->bp.transfer_mode) {
            transc->dma_addr = (uint32_t)buffer;
        }
        usb_transc_inxfer (udev, transc);
    } else {
        usb_transc *transc = &udev->dev.transc_out[ep_num];
        transc->xfer_buf = buffer;
        transc->xfer_len = total_bytes;
        transc->xfer_count = 0U;

        if ((uint8_t)USB_USE_DMA == udev->bp.transfer_mode) {
            transc->dma_addr = (uint32_t)buffer;
        }
        usb_transc_outxfer (udev, transc);
    }

    dcd_debug("dcd_edpt_xfer\n");
    return true;
}

static uint8_t ff_buf[1024];
static tu_fifo_t *cur_ff = NULL;
static bool ff_valid = false;

bool dcd_edpt_xfer_fifo(uint8_t rhport, uint8_t ep_addr, tu_fifo_t * ff, uint16_t total_bytes)
{
    dcd_debug("dcd_edpt_xfer_fifo\n");
    if (total_bytes > 1024) {
        amk_printf("XFER fifo oversize, ep:%d, size:%d\n", ep_addr, total_bytes);
        return false;
    }

    uint8_t const dir = tu_edpt_dir(ep_addr);
    if (dir == TUSB_DIR_OUT) {
        cur_ff = ff;
        ff_valid = true;
    } else {
        tu_fifo_read_n(ff, ff_buf, total_bytes);
    }
    return dcd_edpt_xfer(rhport, ep_addr, ff_buf, total_bytes);
}

void dcd_edpt_stall(uint8_t rhport, uint8_t ep_addr)
{
    usb_core_driver *udev = &usbd_core;
    usb_transc *transc = NULL;

    uint8_t const ep_num = tu_edpt_number(ep_addr);
    uint8_t const ep_dir = tu_edpt_dir(ep_addr);

    if (ep_dir) {
        transc = &udev->dev.transc_in[ep_num];
    } else {
        transc = &udev->dev.transc_out[ep_num];
    }

    transc->ep_stall = 1U;

    usb_transc_stall (udev, transc);
}

void dcd_edpt_clear_stall(uint8_t rhport, uint8_t ep_addr)
{
    usb_core_driver *udev = &usbd_core;
    usb_transc *transc = NULL;

    uint8_t const ep_num = tu_edpt_number(ep_addr);
    uint8_t const ep_dir = tu_edpt_dir(ep_addr);

    if (ep_dir) {
        transc = &udev->dev.transc_in[ep_num];
    } else {
        transc = &udev->dev.transc_out[ep_num];
    }

    transc->ep_stall = 0U;
    usb_transc_clrstall (udev, transc);
}
// usbd callback

uint8_t usbd_bus_reset(void *udev)
{
    dcd_debug("usbd_bus_reset\n");
    dcd_open_ep0(udev);
    dcd_event_bus_signal(0, DCD_EVENT_BUS_RESET, true);
    return 0;
}

uint8_t usbd_unplugged(void *udev)
{
    dcd_event_bus_signal(0, DCD_EVENT_UNPLUGGED, true);
    return 0;
}

static uint8_t usbd_suspend(void *udev)
{
    dcd_event_bus_signal(0, DCD_EVENT_SUSPEND, true);
    return 0;
}

static uint8_t usbd_resume(void *udev)
{
    dcd_event_bus_signal(0, DCD_EVENT_RESUME, true);
    return 0;
}

uint8_t usbd_sof(void *udev)
{
    dcd_event_bus_signal(0, DCD_EVENT_SOF, true);
    return 0;
}

uint8_t usbd_setup_received(void *udev)
{
    dcd_debug("setup_received\n");

    usb_core_driver *core = &usbd_core;

    dcd_event_setup_received(0, (uint8_t*)(&core->dev.control.req), true);
    return 0;
}

uint8_t usbd_xfer_complete(void *udev, uint32_t ep_addr)
{
    dcd_debug("xfer_completed, ep_addr=%d\n", ep_addr);

    usb_core_driver *core = &usbd_core;
    uint8_t const ep_num = tu_edpt_number(ep_addr);
    uint8_t const ep_dir = tu_edpt_dir(ep_addr);
    uint32_t xfer_count = 0;

    if (ep_dir) {
        xfer_count = core->dev.transc_in[ep_num].xfer_count;
    } else {
        xfer_count = core->dev.transc_out[ep_num].xfer_count;
        if (ff_valid) {
            tu_fifo_write_n(cur_ff, ff_buf, xfer_count);
            ff_valid = false;
        }
    }

    dcd_event_xfer_complete(0, ep_addr, xfer_count, XFER_RESULT_SUCCESS, true); 
    return 0;
}
