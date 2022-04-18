/**
 * @file ddl_dcd_hc32.c
 * 
 */

#include "generic_hal.h"
#include "device/dcd.h"
#include "amk_printf.h"
#include "ddl_dcd_int.h"
#include "usb_bsp.h"

static void hd_usb_setup_process(usb_core_instance *pdev);
static void hd_usb_dataout_process(usb_core_instance *pdev, uint8_t epnum);
static void hd_usb_datain_process(usb_core_instance *pdev , uint8_t epnum);
static void hd_usb_sof_process(usb_core_instance *pdev);
static void hd_usb_dev_rst(usb_core_instance *pdev);
static void hd_usb_dev_susp(usb_core_instance *pdev);
static void hd_usb_dev_resume(usb_core_instance *pdev);
static void hd_usb_ctrlconn(usb_core_instance *pdev, uint8_t conn);
static void hd_usb_isoinincomplt_process(usb_core_instance *pdev);
static void hd_usb_isooutincomplt_process(usb_core_instance *pdev);

usb_dev_int_cbk_typedef dev_int_cbk =
{
    &hd_usb_dev_rst,
    &hd_usb_ctrlconn,
    &hd_usb_dev_susp,
    &hd_usb_dev_resume,    
    &hd_usb_sof_process,
    &hd_usb_setup_process,
    &hd_usb_dataout_process,
    &hd_usb_datain_process,   
    &hd_usb_isoinincomplt_process,
    &hd_usb_isooutincomplt_process  
};

usb_dev_int_cbk_typedef  *dev_int_cbkpr = &dev_int_cbk;
usb_core_instance usb_dev;

static void ddl_open_ep(usb_core_instance *pdev, uint8_t ep_addr, uint16_t ep_mps, uint8_t ep_type)
{
    __IO uint8_t ep_dir = tu_edpt_dir(ep_addr);
    __IO uint8_t ep_num = tu_edpt_number(ep_addr);
    
    USB_DEV_EP *ep;
    if(ep_dir == 1) {
        ep = &pdev->dev.in_ep[ep_num];
    }
    else {
        ep = &pdev->dev.out_ep[ep_num];
    }
    
    ep->epidx   = ep_num;
    ep->ep_dir  = ep_dir;
    ep->maxpacket = ep_mps;
    ep->trans_type = ep_type;
    if (ep_dir == 1) {
        /* Assign a Tx FIFO */
        ep->tx_fifo_num = ep_num;
    }
    /* Set initial data PID. */
    if (ep_type == EP_TYPE_BULK) {
        ep->data_pid_start = 0u;
    }
    hd_usb_epactive(pdev, ep);
}

void dcd_init(uint8_t rhport)
{
    usb_core_instance *pdev = &usb_dev;

    hd_usb_bsp_init(pdev);
    hd_usb_setregaddr(pdev);

    pdev->dev.device_cur_status = (uint8_t)USB_DEV_DEFAULT;
    pdev->dev.device_address = 0u;

    for (uint8_t i = 0; i < pdev->basic_cfgs.dev_epnum; i++) {
        USB_DEV_EP* iep = &pdev->dev.in_ep[i];
        iep->ep_dir = 1u;
        iep->epidx = i;
        iep->tx_fifo_num = i;
        iep->trans_type = EP_TYPE_CTRL;
        iep->maxpacket = USB_MAX_EP0_SIZE;
        iep->xfer_buff = 0u;
        iep->xfer_len = 0u;

        USB_DEV_EP* oep = &pdev->dev.out_ep[i];
        oep->ep_dir = 0u;
        oep->epidx = i;
        oep->tx_fifo_num = i;
        oep->trans_type = EP_TYPE_CTRL;
        oep->maxpacket = USB_MAX_EP0_SIZE;
        oep->xfer_buff = 0u;
        oep->xfer_len = 0u;
    }

    hd_usb_gintdis(pdev);
    /*Init the Core (common init.) */
    hd_usb_initusbcore(pdev);
    /* Force Device Mode*/
    hd_usb_modeset(pdev, DEVICE_MODE);
    /* Init Device */
    hd_usb_devmodeinit(pdev);
    /* Enable USB Global interrupt */
    hd_usb_ginten(pdev);

    pdev->dev.device_state   = USB_EP0_IDLE;

    hd_usb_bsp_nvicconfig();

    // force VBUS ON
    HD_USB_WRREG32(&pdev->regs.GREGS->GOTGCTL, 1<<7);
    HD_USB_WRREG32(&pdev->regs.GREGS->GOTGCTL, (1<<7) | (1<<6));
}

void dcd_int_handler(uint8_t rhport)
{
    hd_usb_isr_handler(&usb_dev);
}

void dcd_int_enable(uint8_t rhport)
{
    hd_usb_ginten(&usb_dev);
}

void dcd_int_disable(uint8_t rhport)
{
    hd_usb_gintdis(&usb_dev);
}

void dcd_set_address(uint8_t rhport, uint8_t dev_addr)
{
    usb_core_instance *pdev = &usb_dev;

    hd_usb_devaddrset(pdev, dev_addr);
    dcd_edpt_xfer(0, 0x80u, pdev->dev.setup_pkt_buf, 0u);
    pdev->dev.device_cur_status  = USB_DEV_ADDRESSED;

    amk_printf("\n USB set address: %d\n", dev_addr);
}

void dcd_remote_wakeup(uint8_t rhport)
{
    hd_usb_remotewakeupen(&usb_dev);
}

void dcd_connect(uint8_t rhport)
{
    hd_usb_ctrldevconnect(&usb_dev, 1);
}

void dcd_disconnect(uint8_t rhport)
{
    hd_usb_ctrldevconnect(&usb_dev, 0);
}

// Invoked when a control transfer's status stage is complete.
// May help DCD to prepare for next control transfer, this API is optional.
// void dcd_edpt0_status_complete(uint8_t rhport, tusb_control_request_t const * request) TU_ATTR_WEAK;

bool dcd_edpt_open(uint8_t rhport, tusb_desc_endpoint_t const * p_ep)
{
    usb_core_instance *pdev = &usb_dev;

    ddl_open_ep(pdev , p_ep->bEndpointAddress, p_ep->wMaxPacketSize, p_ep->bmAttributes.xfer);

    return true;
}

void dcd_edpt_close(uint8_t rhport, uint8_t ep_addr)
{
    usb_core_instance *pdev = &usb_dev;
    USB_DEV_EP *ep;
    __IO uint8_t ep_dir = tu_edpt_dir(ep_addr);
    __IO uint8_t ep_num = tu_edpt_number(ep_addr);
    if (ep_addr) {
        ep = &usb_dev.dev.in_ep[ep_num];
    } else {
        ep = &usb_dev.dev.out_ep[ep_num];
    }

    ep->ep_dir  = ep_dir;
    ep->epidx   = ep_num;
    hd_usb_epdeactive(pdev, ep);
}

void dcd_edpt_close_all (uint8_t rhport)
{}

bool dcd_edpt_xfer(uint8_t rhport, uint8_t ep_addr, uint8_t * buffer, uint16_t total_bytes)
{
    usb_core_instance *pdev = &usb_dev;
    __IO uint8_t ep_dir = tu_edpt_dir(ep_addr);
    __IO uint8_t ep_num = tu_edpt_number(ep_addr);
    USB_DEV_EP* ep;
    if (ep_dir) {
        ep = &pdev->dev.in_ep[ep_num];
        ep->dma_addr = (uint32_t)buffer;
    } else {
        ep = &pdev->dev.out_ep[ep_addr];
        if (pdev->basic_cfgs.dmaen == 1u) {
            ep->dma_addr = (uint32_t)buffer;
        }
    }

    ep->xfer_buff = buffer;
    ep->xfer_len = total_bytes;
    ep->xfer_count = 0u;

    ep->ep_dir = ep_dir;
    ep->epidx = ep_num;

    if (ep_num == 0) {
        if (total_bytes == 0) {
            // status stage
            pdev->dev.device_state = ep_dir ? USB_EP0_STATUS_IN : USB_EP0_STATUS_OUT;
        } else {
            // data stage
            pdev->dev.device_state = ep_dir ? USB_EP0_DATA_IN : USB_EP0_DATA_OUT;
        }
        hd_usb_ep0transbegin(pdev, ep);
    } else {
        hd_usb_epntransbegin(pdev, ep);
    }

    return true;
}

static uint8_t ff_buf[1024];
static tu_fifo_t *cur_ff = NULL;
static bool ff_valid = false;

bool dcd_edpt_xfer_fifo(uint8_t rhport, uint8_t ep_addr, tu_fifo_t * ff, uint16_t total_bytes)
{
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
    usb_core_instance *pdev = &usb_dev;
    USB_DEV_EP *ep;
    __IO uint8_t ep_dir = tu_edpt_dir(ep_addr);
    __IO uint8_t ep_num = tu_edpt_number(ep_addr);
    
    if(ep_dir) {
        ep = &pdev->dev.in_ep[ep_num];
    } else {
        ep = &pdev->dev.out_ep[ep_num];
    }

    ep->ep_stall    = 1;
    ep->epidx       = ep_num;
    ep->ep_dir      = ep_dir;

    hd_usb_setepstall(pdev, ep);
}

void dcd_edpt_clear_stall(uint8_t rhport, uint8_t ep_addr)
{
    usb_core_instance *pdev = &usb_dev;
    USB_DEV_EP *ep;
    __IO uint8_t ep_dir = tu_edpt_dir(ep_addr);
    __IO uint8_t ep_num = tu_edpt_number(ep_addr);
    
    if(ep_dir) {
        ep = &pdev->dev.in_ep[ep_num];
    } else {
        ep = &pdev->dev.out_ep[ep_num];
    }

    ep->ep_stall    = 0;
    ep->epidx       = ep_num;
    ep->ep_dir      = ep_dir;

    hd_usb_clearepstall(pdev, ep);
}


//==============================================================
// ddl usb core callback
//
void hd_usb_dataout_process(usb_core_instance *pdev, uint8_t epnum)
{
    USB_DEV_EP* ep = &pdev->dev.out_ep[epnum];
    dcd_event_xfer_complete(0, epnum, ep->xfer_count, XFER_RESULT_SUCCESS, true); 
}

void hd_usb_datain_process(usb_core_instance *pdev , uint8_t epnum)
{
    USB_DEV_EP* ep = &pdev->dev.in_ep[epnum];
    dcd_event_xfer_complete(0, 0x80|epnum, ep->xfer_count, XFER_RESULT_SUCCESS, true); 
}

void hd_usb_sof_process(usb_core_instance *pdev)
{
    dcd_event_bus_signal(0, DCD_EVENT_SOF, true);
}

void hd_usb_setup_process(usb_core_instance *pdev)
{
    dcd_event_setup_received(0, (uint8_t*) &pdev->dev.setup_pkt_buf[0], true);
    pdev->dev.device_state = USB_EP0_SETUP;
}

void hd_usb_dev_rst(usb_core_instance* pdev)
{
    ddl_open_ep(pdev, 0x00u, USB_MAX_EP0_SIZE, EP_TYPE_CTRL);
    ddl_open_ep(pdev, 0x80u, USB_MAX_EP0_SIZE, EP_TYPE_CTRL);
    pdev->dev.device_cur_status = USB_DEV_DEFAULT;
    dcd_event_bus_signal(0, DCD_EVENT_BUS_RESET, true);
    amk_printf("\nUSB RESET \n");
}

void hd_usb_dev_susp(usb_core_instance *pdev)
{
    pdev->dev.device_old_status  = pdev->dev.device_cur_status;
    pdev->dev.device_cur_status  = USB_DEV_SUSPENDED;
    dcd_event_bus_signal(0, DCD_EVENT_SUSPEND, true);
}

void hd_usb_dev_resume(usb_core_instance *pdev)
{
    //pdev->dev.device_cur_status = pdev->dev.device_old_status;
    pdev->dev.device_cur_status = USB_DEV_CONFIGURED;

    dcd_event_bus_signal(0, DCD_EVENT_RESUME, true);
}

void hd_usb_ctrlconn(usb_core_instance *pdev, uint8_t conn)
{
    if(conn) {
        pdev->dev.connection_status = 1u;
    } else {
        pdev->dev.connection_status = 0u;
        dcd_event_bus_signal(0, DCD_EVENT_UNPLUGGED, true);
    }
}

void hd_usb_isoinincomplt_process(usb_core_instance *pdev)
{

}

void hd_usb_isooutincomplt_process(usb_core_instance *pdev)
{
}
