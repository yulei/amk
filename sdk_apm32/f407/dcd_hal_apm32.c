/**
 * @file dcd_hal_apm32.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
*/

#include "generic_hal.h"
#include "device/dcd.h"
#include "amk_printf.h"

#if defined(APM32F407)
    #define DCD_MAX_EP_NUM      6
    #define DCD_USB_INSTANCE    USB_OTG_FS
    #define DCD_USB_IRQn        OTG_HS1_IRQn
#else
    #error "HAL USB unsupported APM32 mcu"
#endif

USBD_HANDLE_T dcd_usb;

static void dcd_open_ep0(USBD_HANDLE_T* usbdh)
{
    amk_printf("open ep0\n");
    // open ep0 here
    USBD_EP_Open(usbdh, 0x00U, TUSB_XFER_CONTROL, CFG_TUD_ENDPOINT0_SIZE);
    USBD_EP_Open(usbdh, 0x80U, TUSB_XFER_CONTROL, CFG_TUD_ENDPOINT0_SIZE);
}

void dcd_init(uint8_t rhport)
{
    /* Configure USB OTG*/
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
        
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_OTG_HS);
        
    /* Link structure */
    dcd_usb.usbGlobal    = USB_OTG_HS;
    dcd_usb.usbDevice    = USB_OTG_HS_D;
    dcd_usb.usbFifo      = USB_OTG_HS_FIFO;
    dcd_usb.usbPower     = USB_OTG_HS_PWR;

    /* Link data */
    dcd_usb.dataPoint    = NULL;

    dcd_usb.usbCfg.mode                = USB_OTG_MODE_DEVICE;
    dcd_usb.usbCfg.phyType             = USB_OTG_PHY_EMB;
    dcd_usb.usbCfg.dmaStatus           = DISABLE;
    dcd_usb.usbCfg.sofStatus           = DISABLE;
    dcd_usb.usbCfg.speed               = USB_OTG_SPEED_HSFSLS;
    dcd_usb.usbCfg.speedChannel        = USBD_SPEED_CH_HS;
    dcd_usb.usbCfg.devEndpointNum      = DCD_MAX_EP_NUM;
    dcd_usb.usbCfg.lowPowerStatus      = DISABLE;
    dcd_usb.usbCfg.powerManageStatus   = DISABLE;
    dcd_usb.usbCfg.vbusSense           = DISABLE;
    dcd_usb.usbCfg.ep1Status           = DISABLE;

    /* NVIC */
    NVIC_EnableIRQRequest(OTG_HS1_IRQn, 1, 0);

    /* Disable USB OTG all global interrupt */
    USB_OTG_DisableAllGlobalInterrupt(dcd_usb.usbGlobal);

    /* Init USB Core */
    USBD_Config(&dcd_usb);

    USBD_Start(&dcd_usb);
}

void dcd_int_handler(uint8_t rhport)
{
    USBD_OTG_IsrHandler(&dcd_usb);
}

void dcd_int_enable (uint8_t rhport)
{
    NVIC_EnableIRQRequest(DCD_USB_IRQn, 1, 0);
}

void dcd_int_disable(uint8_t rhport)
{
    NVIC_DisableIRQRequest(DCD_USB_IRQn);
}

void dcd_set_address(uint8_t rhport, uint8_t dev_addr)
{
    USBD_SetDevAddress(&dcd_usb, dev_addr);
    USBD_EP_Transfer(&dcd_usb, 0, NULL, 0);
}

extern void wait_ms(int ms);

void dcd_remote_wakeup(uint8_t rhport)
{
    USB_OTG_D_EnableRemoteWakeupSignal(dcd_usb.usbDevice);

    wait_ms(5);
    
    USB_OTG_D_DisableRemoteWakeupSignal(dcd_usb.usbDevice);
}

void dcd_connect(uint8_t rhport)
{
    USB_OTG_D_ConfigSoftDisconnect(dcd_usb.usbDevice, USBD_SOFT_DISC_ENABLE);
}

void dcd_disconnect(uint8_t rhport)
{
    USB_OTG_D_ConfigSoftDisconnect(dcd_usb.usbDevice, USBD_SOFT_DISC_NORMAL);
}

// Invoked when a control transfer's status stage is complete.
// May help DCD to prepare for next control transfer, this API is optional.
// void dcd_edpt0_status_complete(uint8_t rhport, tusb_control_request_t const * request) TU_ATTR_WEAK;

bool dcd_edpt_open(uint8_t rhport, tusb_desc_endpoint_t const * p_ep)
{
    USBD_EP_Open(&dcd_usb, p_ep->bEndpointAddress,p_ep->bmAttributes.xfer, p_ep->wMaxPacketSize);

    return true;
}

void dcd_edpt_close(uint8_t rhport, uint8_t ep_addr)
{
    USBD_EP_Close(&dcd_usb, ep_addr);
}

void dcd_edpt_close_all (uint8_t rhport)
{}

bool dcd_edpt_xfer(uint8_t rhport, uint8_t ep_addr, uint8_t * buffer, uint16_t total_bytes)
{
    uint8_t const epnum = tu_edpt_number(ep_addr);
    uint8_t const dir   = tu_edpt_dir(ep_addr);

    if (dir == TUSB_DIR_IN) {
        USBD_EP_Transfer(&dcd_usb, epnum, buffer, total_bytes);
    } else {
        USBD_EP_Receive(&dcd_usb, epnum, buffer, total_bytes);
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
    USBD_EP_Stall(&dcd_usb, ep_addr);
}

void dcd_edpt_clear_stall(uint8_t rhport, uint8_t ep_addr)
{
    USBD_EP_ClearStall(&dcd_usb, ep_addr);
}


// USBD callback override
#define EP_ADDR_MSK     0x0F

void USBD_DisconnectCallback(USBD_HANDLE_T* usbdh)
{
    dcd_event_bus_signal(0, DCD_EVENT_UNPLUGGED, true);
}

void USBD_ConnectCallback(USBD_HANDLE_T* usbdh)
{}

void USBD_ResumeCallback(USBD_HANDLE_T* usbdh)
{
    dcd_event_bus_signal(0, DCD_EVENT_RESUME, true);
}

void USBD_SuspendCallback(USBD_HANDLE_T* usbdh)
{
    dcd_event_bus_signal(0, DCD_EVENT_SUSPEND, true);
}

void USBD_EnumDoneCallback(USBD_HANDLE_T* usbdh)
{
    dcd_open_ep0(usbdh);
    dcd_event_bus_signal(0, DCD_EVENT_BUS_RESET, true);
}

void USBD_SetupStageCallback(USBD_HANDLE_T* usbdh)
{
   dcd_event_setup_received(0, (uint8_t*)usbdh->setup, true);
}

void USBD_DataInStageCallback(USBD_HANDLE_T* usbdh, uint8_t epnum)
{
    dcd_event_xfer_complete(0, 0x80|epnum, usbdh->epOUT[epnum & EP_ADDR_MSK].bufCount, XFER_RESULT_SUCCESS, true); 
}

void USBD_DataOutStageCallback(USBD_HANDLE_T* usbdh, uint8_t epnum)
{
    if (ff_valid) {
        int readed = USBD_EP_ReadRxDataLen(usbdh, epnum);
        tu_fifo_write_n(cur_ff, ff_buf,  readed);
        ff_valid = false;
    }
    dcd_event_xfer_complete(0, epnum, USBD_EP_ReadRxDataLen(usbdh, epnum), XFER_RESULT_SUCCESS, true); 
}

void USBD_SOFCallback(USBD_HANDLE_T* usbdh)
{
    dcd_event_bus_signal(0, DCD_EVENT_SOF, true);
}
