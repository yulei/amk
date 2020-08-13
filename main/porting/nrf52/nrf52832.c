/**
 * nrf52832.c
 * board implementation of nrf52832
 */

#include "ble_config.h"
#include "nrf_pwr_mgmt.h"
#include "app_scheduler.h"
#include "ble_services.h"
#include "ble_adv_service.h"
#include "ble_hids_service.h"
#include "ble_keyboard.h"
#include "eeconfig_fds.h"

ble_driver_t ble_driver = {
    .peer_id = PM_PEER_ID_INVALID,
    .conn_handle = BLE_CONN_HANDLE_INVALID,
    .ble_led = 0,
    .usb_led = 0,
    .vbus_enabled = 0,
#ifdef MATRIX_USE_GPIO 
    .trigger_enabled = 0,
#else
    .trigger_enabled = 1,
#endif
    .output_target = OUTPUT_BLE,
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

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected");
            ble_driver.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            ble_qwr_update_handle(ble_driver.conn_handle);
            //sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_CONN, ble_driver.conn_handle, DEFAULT_TX_POWER_LEVEL);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected");
            // Dequeue all keys without transmission.
            ble_hids_service_flush(false);

            ble_driver.conn_handle = BLE_CONN_HANDLE_INVALID;

            break; // BLE_GAP_EVT_DISCONNECTED

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
            // Send next key event
            ble_hids_service_flush(true);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
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
static void idle_state_handle(void)
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
    
#if CONFIG_JLINK_MONITOR_ENABLED
    NVIC_SetPriority(DebugMonitor_IRQn, _PRIO_SD_LOW);
#endif

    // Initialize.
    log_init();
    timers_init();
    power_management_init();
    ble_stack_init();
    scheduler_init();

    //fds_eeprom_init();

    ble_services_init();
    ble_keyboard_init();

    // Start execution.
    NRF_LOG_INFO("NRF BLE Keyboard started.");
    sd_power_gpregret_get(RST_REGISTER, &reason);
    if (reason & RST_BOOTLOADER) {
        NRF_LOG_INFO("reset to bootloader");

        sd_power_gpregret_clr(RST_REGISTER, RST_BOOTLOADER);
    }

    if (reason & RST_ERASE_BOND) {
        NRF_LOG_INFO("RESET reason: erase bonds");
        erase_bonds = true;
        sd_power_gpregret_clr(RST_REGISTER, RST_ERASE_BOND);
    }

    ble_services_start(erase_bonds);
    ble_keyboard_start();
}

void board_task(void)
{
    idle_state_handle();
}