/**
 * @file generic_nrf52.c
 * board implementation of nrf52
 */

#include "common_config.h"
#include "nrf_pwr_mgmt.h"
#include "app_scheduler.h"
#include "ble_keyboard.h"
#include "gzll_keyboard.h"
#include "eeconfig_fds.h"

#ifdef NRF52840_XXAA
#include "app_usbd.h"
#endif

rf_driver_t rf_driver = {
    .rf_led = 0,
    .usb_led = 0,
    .vbus_enabled = 0,
#ifdef DISABLE_SLEEP
    .sleep_enabled = 0,
#else
    .sleep_enabled = 1,
#endif
    .output_target = OUTPUT_RF,
    .matrix_changed = 0,
    .battery_power = 100,
    .sleep_count = 0,
    .scan_count = 0,
};

bool rf_is_ble = true;

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for the Event Scheduler initialization.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_init(void)
{
    ret_code_t err_code;

    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
    //sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
}


/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
void idle_state_handle(void)
{
    //NRF_LOG_INFO("app scheduler");
    app_sched_execute();
    if (NRF_LOG_PROCESS() == false) {
        nrf_pwr_mgmt_run();
    }
}

void board_init(void)
{
    bool erase_bonds = false;
    uint32_t reason = 0;
    
#if CONFIG_JLINK_MONITOR_ENABLED
    NVIC_SetPriority(DebugMonitor_IRQn, _PRIO_SD_LOW);
#endif

    // Initialize.
    log_init();
    timers_init();
    power_management_init();
    scheduler_init();

#ifdef NRF52840_XXAA
    extern void nrf_app_usbd_init(void);
    nrf_app_usbd_init();
#endif
    reason = NRF_POWER->GPREGRET;
    NRF_LOG_INFO("NRF Keyboard start reason: %x.", reason);
    NRF_POWER->GPREGRET = 0;

    if (reason & RST_ERASE_BOND) {
        NRF_LOG_INFO("RESET reason: erase bonds");
        erase_bonds = true;
    }

    if (GZLL_IS_HOST || GZLL_IS_CLIENT || (reason&RST_USE_GZLL)) {
        NRF_LOG_INFO("use GAZELL protocol");

        //rf_driver.is_ble = 0;
        rf_is_ble = false;
        gzll_keyboard_init(GZLL_IS_HOST);

#ifdef NRF52840_XXAA
        app_usbd_power_events_enable();
#endif
        gzll_keyboard_start();
    } else {
        NRF_LOG_INFO("use BLE protocol");

        //rf_driver.is_ble = 1;
        rf_is_ble = true;
        ble_keyboard_init();

#ifdef NRF52840_XXAA
        app_usbd_power_events_enable();
#endif
        ble_keyboard_start(erase_bonds);
    }
}

void board_task(void)
{
#ifdef NRF52840_XXAA
    while (app_usbd_event_queue_process()) { /* Nothing to do */ }
#endif
    idle_state_handle();
}