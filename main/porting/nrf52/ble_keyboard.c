/**
 * @file ble_keyboard.c
 * 
 */

#include "ble_config.h"

#include "ble_keyboard.h"
#include "ble_adv_service.h"
#include "ble_hids_service.h"
#include "ble_services.h"
#include "amk_eeprom.h"
#include "eeconfig_flash.h"
#include "rf_keyboard.h"

ble_driver_t ble_driver = {
    .peer_id = PM_PEER_ID_INVALID,
    .conn_handle = BLE_CONN_HANDLE_INVALID,
#ifdef MULTI_PEERS
    .current_peer = BLE_PEER_DEVICE_0,
    .restart_advertise = false,
#endif
};

static void ble_send_report(uint8_t type, uint8_t *data, uint8_t size);
static void ble_prepare_sleep(void);

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
            sd_ble_gap_rssi_start(ble_driver.conn_handle, 5, 5);

            ble_qwr_update_handle(ble_driver.conn_handle);
            //sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_CONN, ble_driver.conn_handle, DEFAULT_TX_POWER_LEVEL);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected, conn_handle=%d, reason=%d, clock=%d", p_ble_evt->evt.gap_evt.conn_handle, p_ble_evt->evt.gap_evt.params.disconnected.reason, NRF_CLOCK->LFCLKSRC);
            // Dequeue all keys without transmission.
            ble_hids_service_flush(false);
            sd_ble_gap_rssi_stop(ble_driver.conn_handle);

            ble_driver.conn_handle = BLE_CONN_HANDLE_INVALID;

            #ifdef MULTI_PEERS
            if (ble_driver.restart_advertise) {
                ble_driver.restart_advertise = false;
                ble_adv_service_restart();
                NRF_LOG_INFO("Restart connection");
            }
            #endif

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
            //NRF_LOG_INFO("GAP Event: hvn tx complete.");
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

        case BLE_GAP_EVT_RSSI_CHANGED:
            NRF_LOG_INFO("RSSI changed: %d", p_ble_evt->evt.gap_evt.params.rssi_changed.rssi);
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

void ble_keyboard_init(void)
{
    ble_stack_init();
#ifdef EECONFIG_FLASH
    fds_eeprom_init();
#endif
    ble_services_init();

    rf_keyboard_init(ble_send_report, ble_prepare_sleep);

#ifdef MULTI_PEERS
    ble_driver.current_peer = eeconfig_read_device();
    if (ble_driver.current_peer >= BLE_PEER_DEVICE_MAX) {
        ble_driver.current_peer = BLE_PEER_DEVICE_0;
        eeconfig_update_device(ble_driver.current_peer);
    } 
#endif
}

void ble_keyboard_start(bool erase_bonds)
{
    ble_services_start(erase_bonds);
    rf_keyboard_start();
}

void ble_keyboard_stop(void)
{
    //ble_services_stop();
    //ble_stack_stop();
}

void ble_prepare_sleep(void)
{
    ble_services_prepare_sleep();
}

static void ble_send_report(uint8_t type, uint8_t *data, uint8_t size)
{
    (void)size;
    ble_hids_service_send_report(type, data);
}