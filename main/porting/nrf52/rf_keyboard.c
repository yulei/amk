/**
 * @file rf_keyboard.c
 * 
 */

#include "common_config.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_pwr_mgmt.h"
#include "usb_interface.h"
#include "eeconfig_fds.h"

#include "report.h"
#include "host.h"
#include "keyboard.h"
#include "matrix_scan.h"

#include "rgb_effects.h"

static rf_send_report_t rf_send_report = NULL;
APP_TIMER_DEF(m_keyboard_timer_id);         // keyboard scan timer id

static bool keyboard_pwr_mgmt_shutdown_handler(nrf_pwr_mgmt_evt_t event);
NRF_PWR_MGMT_HANDLER_REGISTER(keyboard_pwr_mgmt_shutdown_handler, NRF_PWR_MGMT_CONFIG_HANDLER_PRIORITY_COUNT - 1);

static void keyboard_timer_init(void);

static void keyboard_timout_handler(void *p_context);
static void keyboard_timer_start(void);
static void keyboard_timer_stop(void);

/** the fllowing function can be overrided by the keyboard codes */
extern void keyboard_set_rgb(bool on);
__attribute__((weak)) void keyboard_prepare_sleep(void)
{}

/* Host driver */
static uint8_t keyboard_leds(void);
static void    send_keyboard(report_keyboard_t *report);
static void    send_mouse(report_mouse_t *report);
static void    send_system(uint16_t data);
static void    send_consumer(uint16_t data);

host_driver_t kbd_driver = {
    .keyboard_leds = keyboard_leds,
    .send_keyboard = send_keyboard,
    .send_mouse = send_mouse,
    .send_system = send_system,
    .send_consumer = send_consumer,
};

static void usb_enabled(void);
static void usb_disabled(void);
static void usb_suspend(bool remote_wakeup_en);
static void usb_resume(void);
static void usb_leds(uint8_t leds);

static nrf_usb_event_handler_t usb_handler = {
    .enable_cb = usb_enabled,
    .disable_cb = usb_disabled,
    .suspend_cb = usb_suspend,
    .resume_cb = usb_resume,
    .leds_cb = usb_leds,
};

void rf_keyboard_init(rf_send_report_t send_report)
{
    rf_send_report = send_report;
    nrf_drv_gpiote_init();

    keyboard_setup();
    keyboard_init();
    host_set_driver(&kbd_driver);
    keyboard_timer_init();
    nrf_usb_init(&usb_handler);
}

void rf_keyboard_start()
{
    keyboard_timer_start();
    rf_driver.scan_count = 0;
}

void rf_keyboard_prepare_sleep(void)
{
    NRF_LOG_INFO("power down sleep preparing");
    // stop all timer
    keyboard_timer_stop();
    app_timer_stop_all();
    nrf_drv_gpiote_uninit();
    // keyboard sleep
    keyboard_prepare_sleep();
    // turn matrix to sense mode
    matrix_prepare_sleep();
    // usb backend to sleep
    nrf_usb_prepare_sleep();
}

void rf_keyboard_jump_bootloader(void)
{
    nrf_usb_reboot();
}

static void keyboard_timer_init(void)
{
    ret_code_t err_code;
    err_code = app_timer_create(&m_keyboard_timer_id,
                            APP_TIMER_MODE_REPEATED,
                            keyboard_timout_handler);
    APP_ERROR_CHECK(err_code);
}


static bool keyboard_rgblight_on(void)
{
#ifdef RGB_EFFECTS_ENABLE
    return rgb_effects_enabled();
#else
    return false;
#endif
}

static bool keyboard_rgbmatrix_on(void)
{
    return false;
}

static bool keyboard_rgb_on(void) { return keyboard_rgblight_on() || keyboard_rgbmatrix_on(); }

static void keyboard_timout_handler(void *p_context)
{
    keyboard_task();

    if (rf_driver.vbus_enabled) {
        // do not make the power related stuff
        return;
    }

    if (rf_driver.matrix_changed) {
        rf_driver.scan_count = 0;
        rf_driver.matrix_changed = 0;
        rf_driver.sleep_count = 0;
    } else {
        if (!keyboard_rgb_on()) {
            rf_driver.scan_count++;
        } else {
            if (rf_driver.battery_power <= BATTERY_LED_THRESHHOLD) {
                keyboard_set_rgb(false);
            }
        }
    }
}

static void keyboard_timer_start(void)
{
    ret_code_t err_code;

    err_code = app_timer_start(m_keyboard_timer_id, KEYBOARD_SCAN_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

static void keyboard_timer_stop(void)
{
    ret_code_t err_code;

    err_code = app_timer_stop(m_keyboard_timer_id);
    APP_ERROR_CHECK(err_code);

}

static uint8_t keyboard_leds(void)
{
    if (rf_driver.output_target == OUTPUT_RF)
        return rf_driver.ble_led;
    
    if (rf_driver.output_target == OUTPUT_USB)
        return rf_driver.usb_led;

    return 0;
}

static void send_keyboard(report_keyboard_t *report)
{
    if (rf_driver.output_target & OUTPUT_RF) {
        rf_send_report(NRF_REPORT_ID_KEYBOARD, &(report->raw[0]), sizeof(report_keyboard_t));
    }
    if (rf_driver.output_target & OUTPUT_USB) {
        nrf_usb_send_report(NRF_REPORT_ID_KEYBOARD, report, sizeof(*report));
    }
}

#ifdef MOUSEKEY_ENABLE

static void send_mouse(report_mouse_t *report)
{
    NRF_LOG_INFO("Send mouse: real");
    if (rf_driver.output_target & OUTPUT_RF) {
        rf_send_report(NRF_REPORT_ID_MOUSE, (uint8_t *)report, sizeof(report_mouse_t));
    }
    if (rf_driver.output_target & OUTPUT_USB) {
        nrf_usb_send_report(NRF_REPORT_ID_MOUSE, report, sizeof(*report));
    }
}

#else

static void send_mouse(report_mouse_t *report)
{ 
    (void)report; 
    NRF_LOG_INFO("Send mouse: fake");
}

#endif

#ifdef EXTRAKEY_ENABLE 

static void send_system(uint16_t data)
{
    NRF_LOG_INFO("Send system: %d", data);
    if (rf_driver.output_target & OUTPUT_RF) {
        rf_send_report(NRF_REPORT_ID_SYSTEM, (uint8_t *)&data, sizeof(data));
    }
    if (rf_driver.output_target & OUTPUT_USB) {
        nrf_usb_send_report(NRF_REPORT_ID_SYSTEM, &data, sizeof(data));
    }
}

static void send_consumer(uint16_t data)
{
    NRF_LOG_INFO("Send consumer: %d", data);
    if (rf_driver.output_target & OUTPUT_RF) {
        rf_send_report(NRF_REPORT_ID_CONSUMER, (uint8_t *)&data, sizeof(data));
    }

    if (rf_driver.output_target & OUTPUT_USB) {
        nrf_usb_send_report(NRF_REPORT_ID_CONSUMER, &data, sizeof(data));
    }
}

#else

static void send_system(uint16_t data)
{
    (void)data;
    NRF_LOG_INFO("Send system: fake");
}

static void send_consumer(uint16_t data)
{
    (void)data;
    NRF_LOG_INFO("Send consumer: fake");
}

#endif

static bool keyboard_pwr_mgmt_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    // currently we did not handle case by case
    switch (event) {
        case NRF_PWR_MGMT_EVT_PREPARE_WAKEUP:
            break;

        case NRF_PWR_MGMT_EVT_PREPARE_SYSOFF:
            break;

        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
            break;

        case NRF_PWR_MGMT_EVT_PREPARE_RESET:
            break;
        default:
            break;
    }
    rf_keyboard_prepare_sleep();
    return true;
}

static void usb_enabled(void)
{
    rf_driver.vbus_enabled = true;
    rf_driver.output_target = OUTPUT_USB;
}

static void usb_disabled(void)
{
    rf_driver.vbus_enabled = false;
    rf_driver.output_target = OUTPUT_RF;
}

static void usb_suspend(bool remote_wakeup_en)
{}

static void usb_resume(void)
{}

static void usb_leds(uint8_t leds)
{
    rf_driver.usb_led = leds;
}

// bluetooth control command
// F21 for select usb/ble output
// F22 for erase bond
// F23 for enter bootloader mode
#include "action.h"
#include "action_layer.h"
bool hook_process_action(keyrecord_t *record) {
{
    if (IS_NOEVENT(record->event) || !record->event.pressed) { 
        return false;
    }
    action_t action = layer_switch_get_action(record->event);
    if (action.kind.id != ACT_MODS) {
        return false;
    }

    switch(action.key.code) {
        case KC_F21: // toggle usb/ble output
            if (rf_driver.output_target == OUTPUT_RF) {
                if (rf_driver.vbus_enabled) {
                    NRF_LOG_INFO("set output to USB");
                    rf_driver.output_target = OUTPUT_USB;
                } else {
                    NRF_LOG_INFO("vbus not enabled, still using BLE");
                }
            } else {
                NRF_LOG_INFO("set output to BLE");
                rf_driver.output_target = OUTPUT_RF;
            }
            return true;

        case KC_F22: // reset to erase bond mode
            NRF_LOG_INFO("reset to erase bond");
            sd_power_gpregret_set(RST_REGISTER, RST_ERASE_BOND);
            sd_nvic_SystemReset();
            return true;

        case KC_F23: // usb mcu to bootloader mode
            NRF_LOG_INFO("send reboot command");
            nrf_usb_reboot();
            return true;
        case KC_F24: // toggle BLE or GAZELL
            if (rf_driver.is_ble) {
                NRF_LOG_INFO("switch to gzll");
                sd_power_gpregret_set(RST_REGISTER, RST_USE_GZLL);
                sd_nvic_SystemReset();
            } else {
                NRF_LOG_INFO("switch to ble");
                NVIC_SystemReset();
            }
            break;
        default:
            break;
        }
    }

    return false;
}