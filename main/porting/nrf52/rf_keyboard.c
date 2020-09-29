/**
 * @file rf_keyboard.c
 * 
 */

#include "common_config.h"
#include "nrf_gpio.h"
#include "nrf_pwr_mgmt.h"
#include "nrfx_gpiote.h"
#include "nrfx_wdt.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_clock.h"
#include "usb_interface.h"
#include "eeconfig_fds.h"

#include "report.h"
#include "host.h"
#include "keyboard.h"
#include "matrix_scan.h"

#include "rgb_effects.h"

static rf_send_report_t rf_send_report = NULL;
static rf_prepare_sleep_t rf_prepare_sleep = NULL;
static nrfx_wdt_channel_id rf_wdt_channgel;

const nrf_drv_timer_t TIMER_KBD = NRF_DRV_TIMER_INSTANCE(1);

//APP_TIMER_DEF(m_keyboard_timer_id);         // keyboard scan timer id

static void rf_wdt_event_handler(void);
static void rf_wdt_init(void);
static void rf_wdt_feed(void);

static bool keyboard_pwr_mgmt_shutdown_handler(nrf_pwr_mgmt_evt_t event);
NRF_PWR_MGMT_HANDLER_REGISTER(keyboard_pwr_mgmt_shutdown_handler, NRF_PWR_MGMT_CONFIG_HANDLER_PRIORITY_COUNT - 1);

static void keyboard_timer_init(void);
static void keyboard_timer_uninit(void);
static void keyboard_timer_start(void);
static void keyboard_timer_stop(void);
static void keyboard_timer_handler(nrf_timer_event_t event_type, void *p_context);

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

void rf_keyboard_init(rf_send_report_t send_report, rf_prepare_sleep_t prepare_sleep)
{
    rf_send_report = send_report;
    rf_prepare_sleep = prepare_sleep;

    nrfx_gpiote_init();

    keyboard_setup();
    keyboard_init();
    host_set_driver(&kbd_driver);
    keyboard_timer_init();
    nrf_usb_init(&usb_handler);
    rf_wdt_init();
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
    keyboard_timer_uninit();
    // stop all timer
    app_timer_stop_all();
    // rf stack sleep
    rf_prepare_sleep();
    // uninit gpiote
    nrfx_gpiote_uninit();
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
    //err_code = app_timer_create(&m_keyboard_timer_id,
    //                        APP_TIMER_MODE_REPEATED,
    //                        keyboard_timout_handler);
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    err_code = nrf_drv_timer_init(&TIMER_KBD, &timer_cfg, keyboard_timer_handler);
    APP_ERROR_CHECK(err_code);

    uint32_t ticks = nrf_drv_timer_ms_to_ticks(&TIMER_KBD, KEYBOARD_SCAN_INTERVAL);

    nrf_drv_timer_extended_compare(&TIMER_KBD, NRF_TIMER_CC_CHANNEL0, ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

}

static void keyboard_timer_start(void)
{
    //ret_code_t err_code;
    //err_code = app_timer_start(m_keyboard_timer_id, KEYBOARD_SCAN_INTERVAL, NULL);
    //APP_ERROR_CHECK(err_code);
    nrf_drv_timer_enable(&TIMER_KBD);
}

static void keyboard_timer_stop(void)
{
    //ret_code_t err_code;
    //err_code = app_timer_stop(m_keyboard_timer_id);
    //APP_ERROR_CHECK(err_code);
    nrf_drv_timer_disable(&TIMER_KBD);
}

static void keyboard_timer_uninit(void)
{
    nrf_drv_timer_uninit(&TIMER_KBD);
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

static void keyboard_timer_handler(nrf_timer_event_t event_type, void *p_context)
{
    if (event_type != NRF_TIMER_EVENT_COMPARE0)
        return;

    NRF_LOG_INFO("Keyboard scan timer handle");
    rf_wdt_feed();
    keyboard_task();
    if (rf_driver.vbus_enabled) {
        // do not run the power related stuff while have usb supply
        return;
    }

    if (rf_driver.matrix_changed) {
        rf_driver.scan_count = 0;
        rf_driver.sleep_count = 0;
        rf_driver.matrix_changed = 0;
    } else {
        if (!keyboard_rgb_on()) {
            rf_driver.scan_count++;
            if (0 == (rf_driver.scan_count % SLEEP_SCAN_OVERFLOW)) {
                rf_driver.sleep_count++;
                NRF_LOG_INFO("Sleep count increased: %d", rf_driver.sleep_count);
            }
            if (rf_driver.sleep_count >= SLEEP_COUNT_THRESHHOLD) {
                if (rf_driver.sleep_enabled) {
                    NRF_LOG_INFO("Sleep count overflow, goto system off mode");
                    nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
                } else {
                    rf_driver.sleep_count = 0;
                }
            }
        } else {
            if (rf_driver.battery_power <= BATTERY_LED_THRESHHOLD) {
                keyboard_set_rgb(false);
            }
        }
    }
}

static uint8_t keyboard_leds(void)
{
    if (rf_driver.output_target == OUTPUT_RF)
        return rf_driver.rf_led;
    
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
            } return true;

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
            } return true;
        default:
            break;
        }
    }

    return false;
}

void rf_wdt_init(void)
{
    ret_code_t err_code;
    if (!nrf_drv_clock_init_check()) {
        err_code = nrf_drv_clock_init();
        APP_ERROR_CHECK(err_code);
    }
    nrf_drv_clock_lfclk_request(NULL);

    nrfx_wdt_config_t config = NRFX_WDT_DEAFULT_CONFIG;
    err_code = nrfx_wdt_init(&config, rf_wdt_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrfx_wdt_channel_alloc(&rf_wdt_channgel);
    APP_ERROR_CHECK(err_code);
    nrfx_wdt_enable();
}

void rf_wdt_feed(void)
{
    nrfx_wdt_channel_feed(rf_wdt_channgel);
}

void rf_wdt_event_handler(void)
{
    NRF_LOG_WARNING("rf watchdog timeout!");
}