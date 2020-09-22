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
    .ble_led = 0,
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
    sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
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
    app_sched_execute();
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


void board_init(void)
{
    bool erase_bonds = false;
    uint32_t reason = 0;
    ret_code_t err_code;
    
#if CONFIG_JLINK_MONITOR_ENABLED
    NVIC_SetPriority(DebugMonitor_IRQn, _PRIO_SD_LOW);
#endif

    // Initialize.
    log_init();
    timers_init();
    power_management_init();
    scheduler_init();

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);
    // Start execution.
    NRF_LOG_INFO("NRF BLE Keyboard started.");
    sd_power_gpregret_get(RST_REGISTER, &reason);

    if (reason & RST_ERASE_BOND) {
        NRF_LOG_INFO("RESET reason: erase bonds");
        erase_bonds = true;
        sd_power_gpregret_clr(RST_REGISTER, RST_ERASE_BOND);
    }

    if (reason & RST_USE_GZLL) {
        NRF_LOG_INFO("use GAZELL protocol");
        sd_power_gpregret_clr(RST_REGISTER, RST_USE_GZLL);
        err_code = nrf_sdh_disable_request();
        APP_ERROR_CHECK(err_code);

        rf_driver.is_ble = 0;
        gzll_keyboard_init();
    } else {
        rf_driver.is_ble = 1;
        ble_keyboard_init();
    }

    /*if (reason & RST_BOOTLOADER) {
        NRF_LOG_INFO("reset to bootloader");
        sd_power_gpregret_clr(RST_REGISTER, RST_BOOTLOADER);
    }*/


    if (reason & RST_USE_GZLL) {
        gzll_keyboard_start();
    } else {
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