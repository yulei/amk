/**
 * usb_interface.h
 *
 */

#pragma once

#include "ble_config.h"

// usb power up
typedef void (*nrf_usb_enable)(void);
// usb power down
typedef void (*nrf_usb_disable)(void);
// usb suspend
typedef void (*nrf_usb_suspend)(bool remote_wakeup_en);
// usb resume
typedef void (*nrf_usb_resume)(void);
// usb leds
typedef void (*nrf_usb_leds)(uint8_t leds);

typedef struct {
    nrf_usb_enable enable_cb;
    nrf_usb_disable disable_cb;
    nrf_usb_suspend suspend_cb;
    nrf_usb_resume resume_cb;
    nrf_usb_leds leds_cb;
} nrf_usb_event_handler_t;

void nrf_usb_preinit(void);
void nrf_usb_init(nrf_usb_event_handler_t *config);
void nrf_usb_postinit(void);
void nrf_usb_task(void);

void nrf_usb_send_report(nrf_report_id report, const void *data, size_t size);

void nrf_usb_prepare_sleep(void);

void nrf_usb_reboot(void);
void nrf_usb_wakeup(void);