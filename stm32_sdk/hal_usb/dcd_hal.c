/**
 * dcd_hal.c
 */
#include "generic_hal.h"
#include "device/dcd.h"
#include "amk_printf.h"

//--------------------------------------------------------------------+
// Controller API
//--------------------------------------------------------------------+
PCD_HandleTypeDef hpcd_USB_OTG_FS;

// Initialize controller to device mode
void dcd_init       (uint8_t rhport)
{
    hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
    hpcd_USB_OTG_FS.Init.dev_endpoints = 4;
    hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
    hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
    hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
    hpcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
    hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
    hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
    hpcd_USB_OTG_FS.Init.vbus_sensing_enable = DISABLE;
    hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
    if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK) {
        amk_printf("Failed to initialize HAL PCD\n");
    }
}

// Interrupt Handler
void dcd_int_handler(uint8_t rhport)
{
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}

// Enable device interrupt
void dcd_int_enable (uint8_t rhport)
{
    HAL_NVIC_SetPriority(OTG_FS_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
}

// Disable device interrupt
void dcd_int_disable(uint8_t rhport)
{
    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
}

// Receive Set Address request, mcu port must also include status IN response
void dcd_set_address(uint8_t rhport, uint8_t dev_addr)
{
    HAL_PCD_SetAddress(&hpcd_USB_OTG_FS, dev_addr);
}

// Wake up host
void dcd_remote_wakeup(uint8_t rhport)
{
    HAL_PCD_ActivateRemoteWakeup(&hpcd_USB_OTG_FS);
    HAL_Delay(5);
    HAL_PCD_DeActivateRemoteWakeup(&hpcd_USB_OTG_FS);
}

// Connect by enabling internal pull-up resistor on D+/D-
void dcd_connect(uint8_t rhport)
{
    HAL_PCD_DevConnect(&hpcd_USB_OTG_FS);
}

// Disconnect by disabling internal pull-up resistor on D+/D-
void dcd_disconnect(uint8_t rhport)
{
    HAL_PCD_DevDisconnect(&hpcd_USB_OTG_FS);
}

//--------------------------------------------------------------------+
// Endpoint API
//--------------------------------------------------------------------+

// Invoked when a control transfer's status stage is complete.
// May help DCD to prepare for next control transfer, this API is optional.
//void dcd_edpt0_status_complete(uint8_t rhport, tusb_control_request_t const * request) TU_ATTR_WEAK;

// Configure endpoint's registers according to descriptor
bool dcd_edpt_open        (uint8_t rhport, tusb_desc_endpoint_t const * p_ep)
{
    HAL_StatusTypeDef status = HAL_PCD_EP_Open(&hpcd_USB_OTG_FS, p_ep->bEndpointAddress, p_ep->wMaxPacketSize.size, p_ep->bmAttributes.xfer);
    if (status != HAL_OK) {
        amk_printf("Failed to open ep:%d, status:%d\n", p_ep->bEndpointAddress, status);
        return false;
    }
    return true;
}

// Close an endpoint.
// Since it is weak, caller must TU_ASSERT this function's existence before calling it.
void dcd_edpt_close        (uint8_t rhport, uint8_t ep_addr)
{
    HAL_PCD_EP_Close(&hpcd_USB_OTG_FS, ep_addr);
}

// Submit a transfer, When complete dcd_event_xfer_complete() is invoked to notify the stack
bool dcd_edpt_xfer        (uint8_t rhport, uint8_t ep_addr, uint8_t * buffer, uint16_t total_bytes)
{
    uint8_t const epnum = tu_edpt_number(ep_addr);
    uint8_t const dir   = tu_edpt_dir(ep_addr);
    HAL_StatusTypeDef status = HAL_OK;
    if (dir == TUSB_DIR_IN) {
        status = HAL_PCD_EP_Receive(&hpcd_USB_OTG_FS, epnum, buffer, total_bytes);
    } else {
        status = HAL_PCD_EP_Transmit(&hpcd_USB_OTG_FS, epnum, buffer, total_bytes);
    }
    if (status != HAL_OK) {
        amk_printf("Failed to xfer ep:%d, status:%d\n", ep_addr, status);
        return false;
    }
    return true;
}

// Stall endpoint
void dcd_edpt_stall       (uint8_t rhport, uint8_t ep_addr)
{
    HAL_PCD_EP_SetStall(&hpcd_USB_OTG_FS, ep_addr);
}

// clear stall, data toggle is also reset to DATA0
void dcd_edpt_clear_stall (uint8_t rhport, uint8_t ep_addr)
{
    HAL_PCD_EP_ClrStall(&hpcd_USB_OTG_FS, ep_addr);
}


//--------------------------------------------------------------------+
// Event API (implemented by stack)
//--------------------------------------------------------------------+

/**
  * @brief  Data OUT stage callback.
  * @param  hpcd PCD handle
  * @param  epnum endpoint number
  * @retval None
  */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    dcd_event_xfer_complete(0, epnum, hpcd->OUT_ep[epnum & EP_ADDR_MSK].xfer_count, XFER_RESULT_SUCCESS, true); 
}

/**
  * @brief  Data IN stage callback
  * @param  hpcd PCD handle
  * @param  epnum endpoint number
  * @retval None
  */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    dcd_event_xfer_complete(0, 0x80 | epnum, hpcd->OUT_ep[epnum & EP_ADDR_MSK].xfer_count, XFER_RESULT_SUCCESS, true); 
}

/**
  * @brief  Setup stage callback
  * @param  hpcd PCD handle
  * @retval None
  */
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
    dcd_event_setup_received(0, (uint8_t*)hpcd->Setup, true);
}

/**
  * @brief  USB Start Of Frame callback.
  * @param  hpcd PCD handle
  * @retval None
  */
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
    dcd_event_bus_signal(0, DCD_EVENT_SOF, true);
}

/**
  * @brief  USB Reset callback.
  * @param  hpcd PCD handle
  * @retval None
  */
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
    dcd_event_bus_signal(0, DCD_EVENT_BUS_RESET, true);
}

/**
  * @brief  Suspend event callback.
  * @param  hpcd PCD handle
  * @retval None
  */
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
    dcd_event_bus_signal(0, DCD_EVENT_SUSPEND, true);
}

/**
  * @brief  Resume event callback.
  * @param  hpcd PCD handle
  * @retval None
  */
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
    dcd_event_bus_signal(0, DCD_EVENT_RESUME, true);
}

/**
  * @brief  Connection event callback.
  * @param  hpcd PCD handle
  * @retval None
  */
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);

    /* NOTE : This function should not be modified, when the callback is needed,
                the HAL_PCD_ConnectCallback could be implemented in the user file
    */
}

/**
  * @brief  Disconnection event callback.
  * @param  hpcd PCD handle
  * @retval None
  */
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
    UNUSED(hpcd);
    dcd_event_bus_signal(0, DCD_EVENT_UNPLUGGED, true);
}

// helper to send bus signal event
//extern void dcd_event_bus_signal (uint8_t rhport, dcd_eventid_t eid, bool in_isr);

// helper to send bus reset event
//extern void dcd_event_bus_reset (uint8_t rhport, tusb_speed_t speed, bool in_isr);

// helper to send setup received
//extern void dcd_event_setup_received(uint8_t rhport, uint8_t const * setup, bool in_isr);

// helper to send transfer complete event
//extern void dcd_event_xfer_complete (uint8_t rhport, uint8_t ep_addr, uint32_t xferred_bytes, uint8_t result, bool in_isr);