/**
 * usb_backend_uart.h
 *      send usb report throught the uart 
 */

#include "usb_interface.h"
#include "nrf_delay.h"
#include "app_usbd.h"

typedef struct {
    nrf_usb_event_handler_t event;
    app_usbd_config_t usbd_config;
} nrf_usb_config_t;

static void usbd_user_ev_handler(app_usbd_event_type_t event);

static nrf_usb_config_t nrf_usb_config = {
    .usbd_config.ev_handler = usbd_user_ev_handler,
};

void nrf_usb_init(nrf_usb_event_handler_t* eh)
{
    ret_code_t rc;
    nrf_usb_config.event.disable_cb = eh->disable_cb;
    nrf_usb_config.event.enable_cb  = eh->enable_cb;
    nrf_usb_config.event.suspend_cb = eh->suspend_cb;
    nrf_usb_config.event.resume_cb  = eh->resume_cb;
    nrf_usb_config.event.leds_cb    = eh->leds_cb;

    rc = app_usbd_init(&nrf_usb_config.usbd_config);
    APP_ERROR_CHECK(rc);
    // attach class instance

    rc = app_usbd_power_events_enable();
    APP_ERROR_CHECK(rc);
}

void nrf_usb_send_report(nrf_report_id report, const void *data, size_t size)
{
    switch(report) {
        case NRF_REPORT_ID_KEYBOARD: 
            NRF_LOG_INFO("Key report:[%x%x]", ((uint32_t*)data)[0], ((uint32_t*)data)[1]);
            break;
#ifdef MOUSEKEY_ENABLE
        case NRF_REPORT_ID_MOUSE:
            NRF_LOG_INFO("Mouse report:");
            for (int i = 0; i < size; i++) {
                NRF_LOG_INFO("0x%x", ((uint8_t*)data)[i]);
            }
            break;
#endif
#ifdef EXTRAKEY_ENABLE
        case NRF_REPORT_ID_SYSTEM:
            NRF_LOG_INFO("system report: 0x%x", *((uint32_t*)data));
            break;
        case NRF_REPORT_ID_CONSUMER:
            //tud_hid_report(REPORT_ID_CONSUMER, &data, sizeof(data));
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

static void usbd_user_ev_handler(app_usbd_event_type_t event)
{
    switch (event)
    {
        case APP_USBD_EVT_DRV_SUSPEND:
            nrf_usb_config.event.suspend_cb(true);
            break;
        case APP_USBD_EVT_DRV_RESUME:
            nrf_usb_config.event.resume_cb();
            break;
        case APP_USBD_EVT_STARTED:
            break;
        case APP_USBD_EVT_STOPPED:
            break;
        case APP_USBD_EVT_POWER_DETECTED:
            if (!nrf_drv_usbd_is_enabled()) {
                app_usbd_enable();
            }
            break;
        case APP_USBD_EVT_POWER_REMOVED:
            nrf_usb_config.event.disable_cb();
            break;
        case APP_USBD_EVT_POWER_READY:
            app_usbd_start();
            nrf_usb_config.event.enable_cb();
            break;
        default:
            break;
    }
}