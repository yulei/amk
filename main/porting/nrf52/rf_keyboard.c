/**
 * @file rf_keyboard.c
 * 
 */

#include "common_config.h"
#include "nrf_gpio.h"
#include "nrf_pwr_mgmt.h"
#include "nrfx_wdt.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_rtc.h"
#include "usb_interface.h"
#include "app_scheduler.h"
#include "ble_adv_service.h"

#include "report.h"
#include "host.h"
#include "keyboard.h"
#include "action.h"
#include "matrix_scan.h"
#include "wait.h"

#include "rgb_led.h"
#include "rf_power.h"
#include "amk_boot.h"
#include "amk_eeprom.h"
#include "amk_keymap.h"
#include "amk_indicator.h"

static rf_send_report_t rf_send_report = NULL;
static rf_prepare_sleep_t rf_prepare_sleep = NULL;
APP_TIMER_DEF(m_keyboard_timer_id);         // keyboard scan timer id

#if WDT_ENABLE
static nrfx_wdt_channel_id rf_wdt_channgel;

static void rf_wdt_event_handler(void);
static void rf_wdt_init(void);
static void rf_wdt_feed(void);
#endif

static bool keyboard_pwr_mgmt_shutdown_handler(nrf_pwr_mgmt_evt_t event);
NRF_PWR_MGMT_HANDLER_REGISTER(keyboard_pwr_mgmt_shutdown_handler, NRF_PWR_MGMT_CONFIG_HANDLER_PRIORITY_COUNT - 1);

static void keyboard_timer_init(void);
static void keyboard_timer_start(void);
static void keyboard_timer_stop(void);
static void keyboard_timer_handler(void *p_context);

/** the fllowing function can be overrided by the keyboard codes */
extern void keyboard_set_rgb(bool on);

__WEAK
void keyboard_prepare_sleep(void)
{
#ifdef RGB_ENABLE
    rgb_led_prepare_sleep();
#endif
}

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

void rf_keyboard_init(rf_send_report_t send_report, rf_prepare_sleep_t prepare_sleep)
{
    rf_send_report = send_report;
    rf_prepare_sleep = prepare_sleep;

    keyboard_setup();
    keyboard_init();
    nrf_usb_init(&usb_handler);
    boot_init();
    host_set_driver(&kbd_driver);
#if WDT_ENABLE
    rf_wdt_init();
#endif
    keyboard_timer_init();
    amk_keymap_init();
    NRF_LOG_INFO("rf keyboard inited");
    amk_indicator_init();
}

void rf_keyboard_start()
{
    keyboard_timer_start();
    rf_driver.scan_count = 0;
}

void rf_keyboard_prepare_sleep(void)
{
    NRF_LOG_INFO("power down sleep preparing");
    // stop keyboard timer
    keyboard_timer_stop();
    // stop all timer
    app_timer_stop_all();
    // rf stack sleep
    rf_prepare_sleep();
    // turn matrix to sense mode
    matrix_prepare_sleep();
    // keyboard sleep
    keyboard_prepare_sleep();
    // usb backend to sleep
    nrf_usb_prepare_sleep();
    // release all active peripherals
    rf_power_prepare_sleep();
}

void rf_keyboard_jump_bootloader(void)
{
    nrf_usb_reboot();
}

bool rf_keyboard_vbus_enabled(void)
{
    return rf_driver.vbus_enabled;
}

static void keyboard_timer_init(void)
{
    ret_code_t err_code;
    err_code = app_timer_create(&m_keyboard_timer_id, APP_TIMER_MODE_REPEATED, keyboard_timer_handler);
    APP_ERROR_CHECK(err_code);
}

static void keyboard_timer_start(void)
{
    ret_code_t err_code;
    err_code = app_timer_start(m_keyboard_timer_id, KEYBOARD_SCAN_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("keyboard timer started");
}

static void keyboard_timer_stop(void)
{
    ret_code_t err_code;
    err_code = app_timer_stop(m_keyboard_timer_id);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("keyboard timer stopped");
}

static void keyboard_timer_handler(void *p_context)
{
    //NRF_LOG_INFO("keyboard timer handler");
#if WDT_ENABLE
    rf_wdt_feed();
#endif
    keyboard_task();

#ifdef RGB_ENABLE
    rgb_led_task();
#endif

    if (rf_driver.vbus_enabled && (rf_driver.output_target&USB_ENABLED)) {
        // do not run the power related stuff while have usb supply
        return;
    }

    if (rf_driver.matrix_changed) {
        rf_driver.scan_count = 0;
        rf_driver.sleep_count = 0;
        rf_driver.matrix_changed = 0;
    } else {
#ifdef RGB_ENABLE
        if (!rgb_led_is_on()) {
#endif
            rf_driver.scan_count++;
            if (0 == (rf_driver.scan_count % SLEEP_SCAN_OVERFLOW)) {
                rf_driver.sleep_count++;
                NRF_LOG_INFO("Sleep count increased: %d", rf_driver.sleep_count);
            }
            if (rf_driver.sleep_count >= SLEEP_COUNT_MAX) {
                if ( rf_driver.output_target & SLEEP_ENABLED) {
                    NRF_LOG_INFO("Sleep count overflow, goto system off mode");
                    nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
                } else {
                    NRF_LOG_INFO("Sleep mode disabled, clear sleep count");
                    rf_driver.sleep_count = 0;
                }
            }
#ifdef RGB_ENABLE
        } else {
            if (rf_driver.battery_power <= BATTERY_LED_THRESHHOLD) {
                rgb_led_set_all(false);
            }
        }
#endif
    }
}

static uint8_t keyboard_leds(void)
{
    if (rf_driver.output_target & OUTPUT_RF)
        return rf_driver.rf_led;
    
    if (rf_driver.output_target & OUTPUT_USB)
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
    if (rf_driver.output_target & USB_ENABLED) {
        rf_driver.vbus_enabled = true;
        rf_driver.output_target &= ~OUTPUT_RF;
        rf_driver.output_target |= OUTPUT_USB;
    }
}

static void usb_disabled(void)
{
    rf_driver.vbus_enabled = false;
    rf_driver.output_target &= ~OUTPUT_USB;
    rf_driver.output_target |= OUTPUT_RF;
}

static void usb_suspend(bool remote_wakeup_en)
{}

static void usb_resume(void)
{}

static void usb_leds(uint8_t leds)
{
    rf_driver.usb_led = leds;
}

static void connect_target(uint8_t device)
{
#ifdef MULTI_PEERS
    if (rf_driver.is_ble) {
        if ((device!=ble_driver.current_peer) && (device<=pm_peer_count())) {
            NRF_LOG_INFO("restart advertising for device:%d, current=%d", device, ble_driver.current_peer);
            ble_driver.current_peer = device;
            eeconfig_update_device(ble_driver.current_peer);
            ble_adv_service_restart();
        }
    }
#endif
}

__WEAK
bool hook_process_action_kb(action_t *action)
{
    return false;
}

// bluetooth control command
// F13 : next rgb config
// F15 : set device 0
// F16 : set device 1
// F17 : set device 2
// F18 : set device 3
// F19 : reset keymap
// F20 : disable sleep mode
// F21 : select usb/rf output
// F22 : erase bond
// F23 : enter bootloader mode
// F24 : toggle ble/gazell mode
#include "action.h"
#include "action_layer.h"
bool hook_process_action_main(keyrecord_t *record)
{
    if (IS_NOEVENT(record->event) || !record->event.pressed) { 
        return false;
    }
    action_t action = layer_switch_get_action(record->event);
    if (action.kind.id != ACT_MODS && action.kind.id != ACT_RMODS) {
        return false;
    }

    switch(action.key.code) {
    //#ifdef RGB_ENABLE
    //    case KC_F13:
    //        rgb_led_config_next();
    //        return true;
    //#endif
        case FN_CONNECT_TARGET_0:
            connect_target(BLE_PEER_DEVICE_0);
            return true;
#ifdef FN_CONNECT_TARGET_1
        case FN_CONNECT_TARGET_1:
            connect_target(BLE_PEER_DEVICE_1);
            return true;
#endif
#ifdef FN_CONNECT_TARGET_2
        case FN_CONNECT_TARGET_2:
            connect_target(BLE_PEER_DEVICE_2);
            return true;
#endif
#ifdef FN_CONNECT_TARGET_3
        case FN_CONNECT_TARGET_3:
            connect_target(BLE_PEER_DEVICE_3);
            return true;
#endif
        //case KC_F19:
            // reset keymap
        //    amk_keymap_reset();
        //    return true;
        //case KC_F20: // disable sleep mode
        //    rf_driver.output_target &= ~SLEEP_ENABLED;
        //    return true;
        case FN_SWITCH_BLE_USB: // toggle usb/ble output
            if (rf_driver.output_target & OUTPUT_RF) {
                if (rf_driver.vbus_enabled) {
                    NRF_LOG_INFO("set output to USB");
                    rf_driver.output_target &= ~OUTPUT_RF;
                    rf_driver.output_target |= OUTPUT_USB;
                } else {
                    NRF_LOG_INFO("vbus not enabled, still using RF");
                }
            } else {
                NRF_LOG_INFO("set output to RF");
                rf_driver.output_target &= ~OUTPUT_USB;
                rf_driver.output_target |= OUTPUT_RF;
            } return true;

        case FN_REBOND: // reset to erase bond mode
            NRF_LOG_INFO("reset to erase bond");
            sd_power_gpregret_set(RST_REGISTER, RST_ERASE_BOND);
            sd_nvic_SystemReset();
            return true;

        case FN_BOOTLOADER: // usb mcu to bootloader mode
            NRF_LOG_INFO("send reboot command");
            nrf_usb_reboot();
            return true;
#ifdef FN_SWITCH_BLE_GZLL
        case FN_SWITCH_BLE_GZLL: // toggle BLE or GAZELL
            if (rf_driver.is_ble) {
                NRF_LOG_INFO("switch to gzll");
                sd_power_gpregret_set(RST_REGISTER, RST_USE_GZLL);
                ret_code_t err_code;
                err_code = nrf_sdh_disable_request();
                APP_ERROR_CHECK(err_code);
                sd_nvic_SystemReset();
            } else {
                NRF_LOG_INFO("switch to ble");
                NRF_POWER->GPREGRET &= ~RST_USE_GZLL;
                NVIC_SystemReset();
            } return true;
#endif
        default:
            break;
    }
    return hook_process_action_kb(&action);
}

#if WDT_ENABLE
void rf_wdt_init(void)
{
    ret_code_t err_code;
    if (!nrf_drv_clock_init_check()) {
        err_code = nrf_drv_clock_init();
        APP_ERROR_CHECK(err_code);
        if (!nrf_drv_clock_lfclk_is_running()) {
            nrf_drv_clock_lfclk_request(NULL);
        }
    }

    nrfx_wdt_config_t config = NRFX_WDT_DEAFULT_CONFIG;
    err_code = nrfx_wdt_init(&config, rf_wdt_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrfx_wdt_channel_alloc(&rf_wdt_channgel);
    APP_ERROR_CHECK(err_code);
    nrfx_wdt_enable();
    NRF_LOG_INFO("WDT enabled");
}

void rf_wdt_feed(void)
{
    nrfx_wdt_channel_feed(rf_wdt_channgel);
}

void rf_wdt_event_handler(void)
{
    NRF_LOG_WARNING("rf watchdog timeout!");
}
#endif