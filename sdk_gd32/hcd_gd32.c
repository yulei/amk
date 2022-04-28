/**
 * @file hcd_gd32.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "tusb_option.h"

#ifndef CFG_TUH_ENABLED
#error "use HOST driver without enable tinyusb host stack"
#endif

#include "gd32e10x.h"
#include "osal/osal.h"

#include "host/hcd.h"
#include "host/usbh.h"

#include "drv_usb_host.h"
#include "drv_usbh_int.h"
#include "usbh_pipe.h"
#include "amk_printf.h"

usb_core_driver usbh_core;

// usbh interrupt callback
static uint8_t hcd_connect_cb(void *uhost);
static uint8_t hcd_disconnect_cb(void *uhost);
static uint8_t hcd_port_enabled_cb(void *uhost);
static uint8_t hcd_port_disabled_cb(void *uhost);
static uint8_t hcd_sof_cb(void *uhost);
static uint8_t hcd_xfer_complete_cb(void *uhost, uint32_t pp_num);

static usbh_int_cb usbh_int_op = {
    hcd_connect_cb,
    hcd_disconnect_cb,
    hcd_port_enabled_cb,
    hcd_port_disabled_cb,
    hcd_sof_cb,
    hcd_xfer_complete_cb,
};

usbh_int_cb *usbh_int_fop = &usbh_int_op;

//--------------------------------------------------------------------+
// HCD API
//--------------------------------------------------------------------+
bool hcd_init(uint8_t rhport)
{
    for (uint8_t i = 0U; i < USBFS_MAX_TX_FIFOS; i++) {
        usbh_core.host.pipe[i].err_count = 0U;
        usbh_core.host.pipe[i].pp_status = PIPE_IDLE;
        usbh_core.host.backup_xfercount[i] = 0U;
    }

    usbh_core.host.pipe[0].ep.mps = 8U;
    usb_basic_init(&usbh_core.bp, &usbh_core.regs, USB_CORE_ENUM_FS);

    usb_globalint_disable(&usbh_core.regs);
    usb_core_init (usbh_core.bp, &usbh_core.regs);
    usb_curmode_set (&usbh_core.regs, HOST_MODE);

    usb_host_init (&usbh_core);

    usb_globalint_enable(&usbh_core.regs);

    return true;
}

void hcd_port_reset(uint8_t rhport)
{
    usb_port_reset(&usbh_core);
}

bool hcd_port_connect_status(uint8_t rhport)
{
    return !(usbh_core.host.connect_status == 0);
}

tusb_speed_t hcd_port_speed_get(uint8_t rhport)
{
    uint32_t port_speed = usb_curspeed_get(&usbh_core);
    if (port_speed == PORT_SPEED_LOW) {
        return TUSB_SPEED_LOW;
    }

    if (port_speed == PORT_SPEED_FULL) {
        return TUSB_SPEED_FULL;
    }

    if (port_speed == PORT_SPEED_HIGH) {
        return TUSB_SPEED_HIGH;
    }

    return TUSB_SPEED_INVALID;
}

// Close all opened endpoint belong to this device
void hcd_device_close(uint8_t rhport, uint8_t dev_addr)
{   
    // clear all pipes
    for (uint8_t i = 0U; i < USBFS_MAX_TX_FIFOS; i++) {
        usbh_core.host.pipe[i].in_used = 0U;
        usbh_core.host.pipe[i].err_count = 0U;
        usbh_core.host.pipe[i].pp_status = PIPE_IDLE;
        usbh_core.host.backup_xfercount[i] = 0U;
    }
    amk_printf("hcd device close\n");
}

uint32_t hcd_frame_number(uint8_t rhport)
{
    return usb_curframe_get(&usbh_core);
}

void hcd_int_enable(uint8_t rhport)
{
    usb_globalint_enable(&usbh_core.regs);
}

void hcd_int_disable(uint8_t rhport)
{
    usb_globalint_disable(&usbh_core.regs);
}

//--------------------------------------------------------------------+
// Endpoint API
//--------------------------------------------------------------------+
#define INVALID_PIPE    0xFF

static uint8_t get_pipe_by_ep_num(usb_core_driver *udev, uint8_t ep_addr)
{
    for (uint8_t i = 0U; i < USBFS_MAX_TX_FIFOS; i++) {
        if (udev->host.pipe[i].in_used) {
            uint8_t ep_dir = tu_edpt_dir(ep_addr);
            uint8_t ep_num = tu_edpt_number(ep_addr);
            if (udev->host.pipe[i].ep.dir == ep_dir && udev->host.pipe[i].ep.num == ep_num) {
                return i;
            }
        }
    }

    return INVALID_PIPE;
}

static bool pipe_data_xfer(usb_core_driver *udev, uint8_t pp_num)
{
    usb_pipe *pp = &udev->host.pipe[pp_num];
    pp->urb_state = URB_IDLE;
    pp->xfer_count = 0U;

    uint32_t status = usb_pipe_xfer(udev, pp_num);
    if (status != USB_OK) {
        amk_printf("GD32 USB HOST pipe xfer failed: %d\n", status);
        return false;
    }
    return true;
}

static bool pipe_data_send(usb_core_driver *udev, uint8_t *buf, uint8_t pp_num, uint16_t len)
{
    usb_pipe *pp = &udev->host.pipe[pp_num];

    pp->xfer_buf = buf;
    pp->xfer_len = len;

    switch (pp->ep.type) {
    case USB_EPTYPE_CTRL:
        if (0U == len) {
            pp->data_toggle_out = 1U;
        }

        pp->DPID = PIPE_DPID[pp->data_toggle_out];
        break;

    case USB_EPTYPE_INTR:
        pp->DPID = PIPE_DPID[pp->data_toggle_out];

        pp->data_toggle_out ^= 1U;
        break;

    case USB_EPTYPE_BULK:
        pp->DPID = PIPE_DPID[pp->data_toggle_out];
        break;

    case USB_EPTYPE_ISOC:
        pp->DPID = PIPE_DPID[0];
        break;

    default:
        break;
    }

    return pipe_data_xfer(udev, pp_num);
}

static bool pipe_data_recv(usb_core_driver *udev, uint8_t *buf, uint8_t pp_num, uint16_t len)
{
    usb_pipe *pp = &udev->host.pipe[pp_num];

    pp->xfer_buf = buf;
    pp->xfer_len = len;

    switch (pp->ep.type) {
    case USB_EPTYPE_CTRL:
        pp->DPID = PIPE_DPID[1];
        break;

    case USB_EPTYPE_INTR:
        pp->DPID = PIPE_DPID[pp->data_toggle_in];

        /* Toggle DATA PID */
        pp->data_toggle_in ^= 1U;
        break;

    case USB_EPTYPE_BULK:
        pp->DPID = PIPE_DPID[pp->data_toggle_in];
        break;

    case USB_EPTYPE_ISOC:
        pp->DPID = PIPE_DPID[0];
        break;

    default:
        break;
    }

    return pipe_data_xfer(udev, pp_num);
}

bool hcd_edpt_open(uint8_t rhport, uint8_t dev_addr, tusb_desc_endpoint_t const * ep_desc)
{
    uint8_t ep_dir = tu_edpt_dir(ep_desc->bEndpointAddress);
    uint8_t ep_num = tu_edpt_number(ep_desc->bEndpointAddress);
    if (ep_num == 0) {
        uint8_t pipe = get_pipe_by_ep_num(&usbh_core, ep_desc->bEndpointAddress);
        if (pipe == INVALID_PIPE) {
            pipe = usbh_pipe_allocate(&usbh_core, 0);
            usbh_pipe_create(&usbh_core, dev_addr, hcd_port_speed_get(0), pipe, ep_desc->bmAttributes.xfer, ep_desc->wMaxPacketSize);
            //usbh_pipe_toggle_set(&usbh_core, pipe, 0);

            pipe = usbh_pipe_allocate(&usbh_core, 0x80);
            usbh_pipe_create(&usbh_core, dev_addr, hcd_port_speed_get(0), pipe, ep_desc->bmAttributes.xfer, ep_desc->wMaxPacketSize);
            //usbh_pipe_toggle_set(&usbh_core, pipe, 0);
            amk_printf("hcd open control ep: dir=%d, num=%d\n", ep_dir, ep_num);
        } else {
            pipe = get_pipe_by_ep_num(&usbh_core, 0x00);
            usbh_pipe_update (&usbh_core, pipe, dev_addr, hcd_port_speed_get(0), ep_desc->wMaxPacketSize);
            pipe = get_pipe_by_ep_num(&usbh_core, 0x80);
            usbh_pipe_update (&usbh_core, pipe, dev_addr, hcd_port_speed_get(0), ep_desc->wMaxPacketSize);
            amk_printf("hcd update control ep: dir=%d, num=%d\n", ep_dir, ep_num);
        }
    } else {
        uint8_t pipe = usbh_pipe_allocate(&usbh_core, tu_edpt_addr(ep_num, ep_dir));
        usbh_pipe_create(&usbh_core, dev_addr, hcd_port_speed_get(0), pipe, ep_desc->bmAttributes.xfer, ep_desc->wMaxPacketSize);
        usbh_pipe_toggle_set(&usbh_core, pipe, 0);
        amk_printf("hcd open data ep: dir=%d, num=%d\n", ep_dir, ep_num);
    }
    return true;
}

bool hcd_edpt_xfer(uint8_t rhport, uint8_t dev_addr, uint8_t ep_addr, uint8_t * buffer, uint16_t buflen)
{
    uint8_t pp_num = get_pipe_by_ep_num(&usbh_core, ep_addr);
    uint8_t ep_dir = tu_edpt_dir(ep_addr);
    if (ep_dir == TUSB_DIR_IN) {
        return pipe_data_recv(&usbh_core, buffer, pp_num, buflen);
    } else {
        return pipe_data_send(&usbh_core, buffer, pp_num, buflen);
    }
}

bool hcd_setup_send(uint8_t rhport, uint8_t dev_addr, uint8_t const setup_packet[8])
{
    uint8_t pp_num = get_pipe_by_ep_num(&usbh_core, 0x00);

    usb_pipe *pp = &usbh_core.host.pipe[pp_num];

    pp->DPID = PIPE_DPID_SETUP;
    pp->xfer_buf = (uint8_t*)&setup_packet[0];
    pp->xfer_len = USB_SETUP_PACKET_LEN;

    return pipe_data_xfer(&usbh_core, pp_num);
}

bool hcd_edpt_clear_stall(uint8_t dev_addr, uint8_t ep_addr)
{
    return true;
}

// hcd interrupt callback
uint8_t hcd_connect_cb(void *uhost)
{
    amk_printf("GD32 HCD: connect callback\n");
    hcd_port_reset(0);
    return 0;
}

uint8_t hcd_disconnect_cb(void *uhost)
{
    amk_printf("GD32 HCD: disconnect callback\n");
    hcd_event_device_remove(0, true);
    return 0;
}

uint8_t hcd_port_enabled_cb(void *uhost)
{
    amk_printf("GD32 HCD: PORT enable callback\n");
    hcd_event_device_attach(0, true);
    return 0;
}

uint8_t hcd_port_disabled_cb(void *uhost)
{
    amk_printf("GD32 HCD: PORT disable callback\n");
    return 0;
}

uint8_t hcd_sof_cb(void *uhost)
{
    //amk_printf("GD32 HCD: SOF callback\n");
    return 0;
}

uint8_t hcd_xfer_complete_cb(void *uhost, uint32_t pp_num)
{
    usb_pipe *pp = &usbh_core.host.pipe[pp_num];

    hcd_event_xfer_complete(pp->dev_addr, tu_edpt_addr(pp->ep.num, pp->ep.dir), pp->xfer_len, XFER_RESULT_SUCCESS, true);

    return 0;
}