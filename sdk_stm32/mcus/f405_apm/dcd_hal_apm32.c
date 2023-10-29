/**
 * @file dcd_hal_apm32.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
*/

#include "apm32f4xx_misc.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_usb.h"
#include "apm32f4xx_usb_device.h"

#include "device/dcd.h"
#include "amk_printf.h"

#if defined(APM32F407)
    #define DCD_MAX_EP_NUM      6
    #define DCD_USB_INSTANCE    USB_OTG_FS
    #define DCD_USB_IRQn        OTG_HS1_IRQn
#else
    #error "HAL USB unsupported APM32 mcu"
#endif

__attribute__((noinline)) static void usb_busy_delay_us(uint32_t us)
{
    __IO uint32_t ticks = us * (SystemCoreClock/1000000);
    while( ticks--) ;
}

__attribute__((noinline)) static void usb_busy_delay_ms(uint32_t ms)
{
    for (int i = 0; i < ms; i++) {
        usb_busy_delay_us(1000);
    }
}

USBD_HANDLE_T dcd_usb;

__attribute__((noinline)) static void dcd_open_ep0(USBD_HANDLE_T* usbdh)
{
    amk_printf("open ep0\n");
    // open ep0 here
    USBD_EP_Open(usbdh, 0x00U, TUSB_XFER_CONTROL, CFG_TUD_ENDPOINT0_SIZE);
    USBD_EP_Open(usbdh, 0x80U, TUSB_XFER_CONTROL, CFG_TUD_ENDPOINT0_SIZE);
}

__attribute__((noinline)) static void my_usbd_config(USBD_HANDLE_T* usbdh)
{
    uint8_t i;
    
    /* Embedded PHY */
    if (usbdh->usbCfg.phyType == USB_OTG_PHY_EMB)
    {
        /* Only Use Embedded HS2 PHY */
        {
            USB_OTG_HS2->USB_SWITCH_B.usb_switch = BIT_SET;
            USB_OTG_HS2->POWERON_CORE_B.poweron_core = BIT_SET;
            USB_OTG_HS2->OTG_SUSPENDM_B.otg_suspendm = BIT_SET;
            USB_OTG_HS2->SW_RREF_I2C_B.sw_rref_i2c = 0x05;
            
            USB_OTG_ConfigPHY(usbdh->usbGlobal, USB_OTG_PHY_SP_HS);

            /* Reset core */
            USB_OTG_CoreReset(usbdh->usbGlobal);
            //APM_DelayMs(50);
            usb_busy_delay_ms(50);
            
            /* battery status */
            if (usbdh->usbCfg.batteryStatus == ENABLE)
            {
                /* Activate the power down*/
                USB_OTG_EnablePowerDown(usbdh->usbGlobal);
            }
            else
            {
                /* Deactivate the power down*/
                USB_OTG_DisablePowerDown(usbdh->usbGlobal);
            }
        }
    }

    if (usbdh->usbCfg.dmaStatus == ENABLE)
    {
        USB_OTG_SetBurstLenght(usbdh->usbGlobal, USB_OTG_BURST_INCR4);

        USB_OTG_EnableDMA(usbdh->usbGlobal);
    }

    /* Device mode or host mode */
    USB_OTG_ConfigMode(usbdh->usbGlobal, usbdh->usbCfg.mode);

    /* Init control endpoint structure */
    for (i = 0; i < usbdh->usbCfg.devEndpointNum; i++)
    {
        /* OUT control endpoint */
        usbdh->epOUT[i].epNum       = i;
        usbdh->epOUT[i].epDir       = EP_DIR_OUT;

        usbdh->epOUT[i].epType      = EP_TYPE_CONTROL;
        usbdh->epOUT[i].mps         = 0;
        usbdh->epOUT[i].buffer      = 0;
        usbdh->epOUT[i].bufLen      = 0;

        /* IN control endpoint */
        usbdh->epIN[i].epNum        = i;
        usbdh->epIN[i].epDir        = EP_DIR_IN;
        usbdh->epIN[i].txFifoNum    = i;

        usbdh->epIN[i].epType       = EP_TYPE_CONTROL;
        usbdh->epIN[i].mps          = 0;
        usbdh->epIN[i].buffer       = 0;
        usbdh->epIN[i].bufLen       = 0;
    }

    /* Init address */
    usbdh->address = 0;

    /* Init device control register */

    /* Clear IN endpoint FIFO */
    for (i = 0; i < 0x0F; i++)
    {
        usbdh->usbGlobal->DTXFIFO[i].word = 0;
    }

    /* Configure VBUS sense */
    if (usbdh->usbCfg.vbusSense == ENABLE)
    {
        usbdh->usbGlobal->GGCCFG_B.VBSDIS = BIT_RESET;
        usbdh->usbGlobal->GGCCFG_B.BDVBSEN = BIT_SET;
    }
    else
    {
        /* VBUS Sensing Disable */
        USB_OTG_D_ConfigSoftDisconnect(usbdh->usbDevice, USBD_SOFT_DISC_ENABLE);

        usbdh->usbGlobal->GGCCFG_B.VBSDIS = BIT_SET;
        usbdh->usbGlobal->GGCCFG_B.ADVBSEN = BIT_RESET;
        usbdh->usbGlobal->GGCCFG_B.BDVBSEN = BIT_RESET;
    }

    /* PHY Clock restart */
    usbdh->usbPower->PCGCTRL = 0;

    /* Configure device parameters */
    USB_OTG_D_ConfigPeriodicFrameInterval(usbdh->usbDevice, USBD_FRAME_INTERVAL_80);
    
    if(usbdh->usbCfg.phyType == USB_OTG_PHY_EMB)
    {
        if (usbdh->usbCfg.speed == USB_OTG_SPEED_FSLS)
        {
            if(usbdh->usbCfg.speedChannel == USBD_SPEED_CH_FS)
            {
                USB_OTG_D_ConfigDeviceSpeed(usbdh->usbDevice, USBD_DEV_SPEED_FS);
            }
            else
            {
                USB_OTG_D_ConfigDeviceSpeed(usbdh->usbDevice, USBD_DEV_SPEED_HS_IN_FS);
            }
        }
        else
        {
            USB_OTG_D_ConfigDeviceSpeed(usbdh->usbDevice, USBD_DEV_SPEED_HS);
        }
    }
    else
    {
        if (usbdh->usbCfg.speed == USB_OTG_SPEED_FSLS)
        {
            USB_OTG_D_ConfigDeviceSpeed(usbdh->usbDevice, USBD_DEV_SPEED_HS_IN_FS);
        }
        else
        {
            USB_OTG_D_ConfigDeviceSpeed(usbdh->usbDevice, USBD_DEV_SPEED_HS);
        }
    }

    /* Flush all Tx and Rx FIFOs */
    USB_OTG_FlushTxFIFO(usbdh->usbGlobal, 0x10);
    USB_OTG_FlushRxFIFO(usbdh->usbGlobal);

    USB_OTG_D_DisableAllInEpInterrupt(usbdh->usbDevice);
    USB_OTG_D_DisableAllOutEpInterrupt(usbdh->usbDevice);
    USB_OTG_D_DisableAllEpInterrupt(usbdh->usbDevice);

    /* Init endpoint */
    for (i = 0; i < usbdh->usbCfg.devEndpointNum; i++)
    {
        /* Reset OUT endpoint */
        if (usbdh->usbDevice->EP_OUT[i].DOEPCTRL_B.EPEN)
        {
            if (i != 0)
            {
                usbdh->usbDevice->EP_OUT[i].DOEPCTRL_B.EPDIS = BIT_SET;
            }

            usbdh->usbDevice->EP_OUT[i].DOEPCTRL_B.NAKSET = BIT_SET;
        }
        else
        {
            usbdh->usbDevice->EP_OUT[i].DOEPCTRL = 0;
        }

        /* Reset transfer size */
        usbdh->usbDevice->EP_OUT[i].DOEPTRS = 0;

        /* Clear endpoint Flag */
        usbdh->usbDevice->EP_OUT[i].DOEPINT = 0xFB7F;

        /* Reset IN endpoint */
        if (usbdh->usbDevice->EP_IN[i].DIEPCTRL_B.EPEN)
        {
            if (i != 0)
            {
                usbdh->usbDevice->EP_IN[i].DIEPCTRL_B.EPDIS = BIT_SET;
            }

            usbdh->usbDevice->EP_IN[i].DIEPCTRL_B.NAKSET = BIT_SET;
        }
        else
        {
            usbdh->usbDevice->EP_IN[i].DIEPCTRL = 0;
        }

        /* Reset transfer size */
        usbdh->usbDevice->EP_IN[i].DIEPTRS = 0;

        /* Clear endpoint Flag */
        usbdh->usbDevice->EP_IN[i].DIEPINT = 0xFB7F;
    }

    USB_OTG_D_DisableInEpInterrupt(usbdh->usbDevice, USBD_INT_EP_IN_TXFUDR);

    USB_OTG_DisableAllGlobalInterrupt(usbdh->usbGlobal);

    USB_OTG_ClearGlobalIntFlag(usbdh->usbGlobal, 0xBFFFFFFF);


    USB_OTG_ConfigRxFifoSize(usbdh->usbGlobal, 0x80);

    USBD_OTG_ConfigDeviceTxFifo(usbdh, 0, 0x20);
    USBD_OTG_ConfigDeviceTxFifo(usbdh, 1, 0x10);
    USBD_OTG_ConfigDeviceTxFifo(usbdh, 2, 0x10);
    USBD_OTG_ConfigDeviceTxFifo(usbdh, 3, 0x80);

#if 0
    /* Configure FIFO */
    if (usbdh->usbCfg.speed == USB_OTG_SPEED_FSLS)
    {
        /* Configure RX FIFO */
        USB_OTG_ConfigRxFifoSize(usbdh->usbGlobal, USBD_FS_RX_FIFO_SIZE);

        /* Configure TX FIFO */
        USBD_OTG_ConfigDeviceTxFifo(usbdh, 0, USBD_FS_TX_FIFO_0_SIZE);
        USBD_OTG_ConfigDeviceTxFifo(usbdh, 1, USBD_FS_TX_FIFO_1_SIZE);
    }
    else
    {
        /* Configure RX FIFO */
        USB_OTG_ConfigRxFifoSize(usbdh->usbGlobal, USBD_HS_RX_FIFO_SIZE);

        /* Configure TX FIFO */
        USBD_OTG_ConfigDeviceTxFifo(usbdh, 0, USBD_HS_TX_FIFO_0_SIZE);
        USBD_OTG_ConfigDeviceTxFifo(usbdh, 1, USBD_HS_TX_FIFO_1_SIZE);
    }
#endif

    if (usbdh->usbCfg.dmaStatus == DISABLE)
    {
        usbdh->usbGlobal->GINTMASK |= USB_INT_G_RXFNONE;
    }

    if (usbdh->usbCfg.sofStatus == ENABLE)
    {
        usbdh->usbGlobal->GINTMASK |= USB_INT_G_SOF;
    }

    if (usbdh->usbCfg.vbusSense == ENABLE)
    {
        usbdh->usbGlobal->GINTMASK |= USB_INT_G_OTG | USB_INT_G_SREQ;
    }

    /* Enable the common interrupts */
    usbdh->usbGlobal->GINTMASK |= (USB_INT_G_USBSUS     |
                                   USB_INT_G_USBRST     |
                                   USB_INT_G_ENUMD      |
                                   USB_INT_G_INEP       |
                                   USB_INT_G_ONEP       |
                                   USB_INT_G_IIINTX     |
                                   USB_INT_G_IP_OUTTX   |
                                   USB_INT_G_RWAKE);

    /* Reset PHY, gate and PHY CLK */
    usbdh->usbPower->PCGCTRL_B.PCLKSTOP = BIT_RESET;
    usbdh->usbPower->PCGCTRL_B.GCLK = BIT_RESET;

    /* Enable soft disconnect */
    USB_OTG_D_ConfigSoftDisconnect(usbdh->usbDevice, USBD_SOFT_DISC_ENABLE);
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
#ifdef USE_HS_USB
    dcd_usb.usbCfg.speed               = USB_OTG_SPEED_HSFSLS;
    dcd_usb.usbCfg.speedChannel        = USBD_SPEED_CH_HS;
#else
    dcd_usb.usbCfg.speed               = USB_OTG_SPEED_FSLS;
    dcd_usb.usbCfg.speedChannel        = USBD_SPEED_CH_HS;//USBD_SPEED_CH_FS;
#endif
    dcd_usb.usbCfg.devEndpointNum      = DCD_MAX_EP_NUM;
    dcd_usb.usbCfg.lowPowerStatus      = DISABLE;
    dcd_usb.usbCfg.powerManageStatus   = DISABLE;
    dcd_usb.usbCfg.vbusSense           = DISABLE;
    dcd_usb.usbCfg.ep1Status           = DISABLE;

    /* NVIC */
    NVIC_EnableIRQRequest(DCD_USB_IRQn, 1, 0);

    /* Disable USB OTG all global interrupt */
    USB_OTG_DisableAllGlobalInterrupt(dcd_usb.usbGlobal);

    /* Init USB Core */
    my_usbd_config(&dcd_usb);
    //USBD_Config(&dcd_usb);

    usb_busy_delay_ms(10);

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
    dcd_event_xfer_complete(0, 0x80|epnum, usbdh->epIN[epnum & EP_ADDR_MSK].bufCount, XFER_RESULT_SUCCESS, true); 
}

void USBD_DataOutStageCallback(USBD_HANDLE_T* usbdh, uint8_t epnum)
{
    /*
    if (ff_valid) {
        int readed = USBD_EP_ReadRxDataLen(usbdh, epnum);
        tu_fifo_write_n(cur_ff, ff_buf,  readed);
        ff_valid = false;
    }
    */
    dcd_event_xfer_complete(0, epnum & EP_ADDR_MSK, USBD_EP_ReadRxDataLen(usbdh, epnum & EP_ADDR_MSK), XFER_RESULT_SUCCESS, true); 
}

void USBD_SOFCallback(USBD_HANDLE_T* usbdh)
{
    dcd_event_bus_signal(0, DCD_EVENT_SOF, true);
}
