/**
 * usb_backend_uart.h
 *      send usb report throught the uart 
 */

#include "usb_interface.h"
#include "nrf_delay.h"
#include "tusb.h"

typedef struct {
    nrf_usb_event_handler_t event;
    bool vbus_enabled;
} nrf_usb_config_t;

static nrf_usb_config_t usb_config;

static void usb_update_state();

void nrf_usb_init(nrf_usb_event_handler_t* eh)
{
    usb_config.event.enable_cb  = eh->enable_cb;
    usb_config.event.disable_cb = eh->disable_cb;
    usb_config.event.suspend_cb = eh->suspend_cb;
    usb_config.event.resume_cb  = eh->resume_cb;
    usb_config.event.leds_cb    = eh->leds_cb;

    // Priorities 0, 1, 4 (nRF52) are reserved for SoftDevice
    // 2 is highest for application
    NVIC_SetPriority(USBD_IRQn, 2);

    // USB power may already be ready at this time -> no event generated
    // We need to invoke the handler based on the status initially
    uint32_t usb_reg;

    sd_power_usbdetected_enable(true);
    sd_power_usbpwrrdy_enable(true);
    sd_power_usbremoved_enable(true);
    sd_power_usbregstatus_get(&usb_reg);

    if ( usb_reg & POWER_USBREGSTATUS_VBUSDETECT_Msk ) tusb_hal_nrf_power_event(NRFX_POWER_USB_EVT_DETECTED);
    if ( usb_reg & POWER_USBREGSTATUS_OUTPUTRDY_Msk  ) tusb_hal_nrf_power_event(NRFX_POWER_USB_EVT_READY);

    usb_update_state();
}

void nrf_usb_send_report(nrf_report_id report, const void *data, size_t size)
{
    switch(report) {
        case NRF_REPORT_ID_KEYBOARD: 
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, report->mods, report->keys);
            NRF_LOG_INFO("Key report:[%x%x]", ((uint32_t*)data)[0], ((uint32_t*)data)[1]);
            break;
#ifdef MOUSEKEY_ENABLE
        case NRF_REPORT_ID_MOUSE:
            tud_hid_mouse_report(REPORT_ID_MOUSE, report->buttons, report->x, report->y, report->v, report->h);
            NRF_LOG_INFO("Mouse report:");
            for (int i = 0; i < size; i++) {
                NRF_LOG_INFO("0x%x", ((uint8_t*)data)[i]);
            }
            break;
#endif
#ifdef EXTRAKEY_ENABLE
        case NRF_REPORT_ID_SYSTEM:
            tud_hid_report(REPORT_ID_SYSTEM, &data, sizeof(data));
            NRF_LOG_INFO("system report: 0x%x", *((uint32_t*)data));
            break;
        case NRF_REPORT_ID_CONSUMER:
            tud_hid_report(REPORT_ID_CONSUMER, &data, sizeof(data));
            NRF_LOG_INFO("consumer report: 0x%x", *((uint32_t*)data));
            break;
#endif
        default:
            NRF_LOG_INFO("Unknow report id: %d", report);
            break;
    }
}

void nrf_usb_wakeup(void) {}

void nrf_usb_prepare_sleep(void) { }

void nrf_usb_reboot(void) { }
