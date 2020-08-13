/**
 * @file ble_adv_service.h
 * @brief ble advertising service
 */
#include <stdint.h>
#include <string.h>

#include "ble_advertising.h"
#include "ble_advdata.h"

#include "ble_adv_service.h"
#include "ble_services.h"

#include "nrf_pwr_mgmt.h"

static ble_uuid_t m_adv_uuids[] = {{BLE_UUID_HUMAN_INTERFACE_DEVICE_SERVICE, BLE_UUID_TYPE_BLE}};

BLE_ADVERTISING_DEF(m_advertising);                                 /**< Advertising module instance. */

static void on_adv_evt(ble_adv_evt_t ble_adv_evt);
static void adv_error_handler(uint32_t nrf_error);
static void sleep_mode_enter(void);

void ble_adv_service_init(void) {
    uint32_t                err_code;
    ble_advertising_init_t  init;
    static int8_t           tx_power = DEFAULT_TX_POWER_LEVEL;

    memset(&init, 0, sizeof(init));
    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;
    init.advdata.p_tx_power_level        = &tx_power;

    init.config.ble_adv_whitelist_enabled          = true;
    init.config.ble_adv_directed_high_duty_enabled = true;
    init.config.ble_adv_directed_enabled           = false;
    init.config.ble_adv_directed_interval          = 0;
    init.config.ble_adv_directed_timeout           = 0;
    init.config.ble_adv_fast_enabled               = true;
    init.config.ble_adv_fast_interval              = APP_ADV_FAST_INTERVAL;
    init.config.ble_adv_fast_timeout               = APP_ADV_FAST_DURATION;
    init.config.ble_adv_slow_enabled               = true;
    init.config.ble_adv_slow_interval              = APP_ADV_SLOW_INTERVAL;
    init.config.ble_adv_slow_timeout               = APP_ADV_SLOW_DURATION;

    init.evt_handler   = on_adv_evt;
    init.error_handler = adv_error_handler;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
    
     err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_advertising.adv_handle, DEFAULT_TX_POWER_LEVEL);
    APP_ERROR_CHECK(err_code);
}

void ble_adv_service_start(bool erase_bonds) {
    if (erase_bonds == true) {
        ble_pm_delete_bonds();
        // Advertising is started by PM_EVT_PEERS_DELETE_SUCCEEDED event.
    } else {
        ble_pm_whitelist_set(PM_PEER_ID_LIST_SKIP_NO_ID_ADDR);

        ret_code_t ret = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
        APP_ERROR_CHECK(ret);
    }
}

/**@brief Function for handling advertising errors.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void adv_error_handler(uint32_t nrf_error) {
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt) {
    ret_code_t err_code;

    switch (ble_adv_evt) {
    case BLE_ADV_EVT_DIRECTED_HIGH_DUTY:
        NRF_LOG_INFO("High Duty Directed advertising.");
        break;

    case BLE_ADV_EVT_DIRECTED:
        NRF_LOG_INFO("Directed advertising.");
        break;

    case BLE_ADV_EVT_FAST:
        NRF_LOG_INFO("Fast advertising.");
        break;

    case BLE_ADV_EVT_SLOW:
        NRF_LOG_INFO("Slow advertising.");
        break;

    case BLE_ADV_EVT_FAST_WHITELIST:
        NRF_LOG_INFO("Fast advertising with whitelist.");
        break;

    case BLE_ADV_EVT_SLOW_WHITELIST:
        NRF_LOG_INFO("Slow advertising with whitelist.");
        break;

    case BLE_ADV_EVT_IDLE:
        if (ble_driver.vbus_enabled) {
            ble_advertising_start(&m_advertising, BLE_ADV_MODE_SLOW);
        } else {
            NRF_LOG_INFO("Enter sleep mode.");
            sleep_mode_enter();
        }
        break;

    case BLE_ADV_EVT_WHITELIST_REQUEST: {
            ble_gap_addr_t whitelist_addrs[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
            ble_gap_irk_t  whitelist_irks[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
            uint32_t       addr_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;
            uint32_t       irk_cnt  = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

            err_code = pm_whitelist_get(whitelist_addrs, &addr_cnt, whitelist_irks,  &irk_cnt);
            APP_ERROR_CHECK(err_code);
            NRF_LOG_DEBUG("pm_whitelist_get returns %d addr in whitelist and %d irk whitelist", addr_cnt, irk_cnt);
            // Set the correct identities list (no excluding peers with no Central Address Resolution).
            ble_pm_identities_set(PM_PEER_ID_LIST_SKIP_NO_IRK);

            // Apply the whitelist.
            err_code = ble_advertising_whitelist_reply(&m_advertising, whitelist_addrs, addr_cnt, whitelist_irks, irk_cnt);
            APP_ERROR_CHECK(err_code);
        }
        break; //BLE_ADV_EVT_WHITELIST_REQUEST

    case BLE_ADV_EVT_PEER_ADDR_REQUEST: {
            pm_peer_data_bonding_t peer_bonding_data;

            // Only Give peer address if we have a handle to the bonded peer.
            if (ble_driver.peer_id != PM_PEER_ID_INVALID) {
                err_code = pm_peer_data_bonding_load(ble_driver.peer_id, &peer_bonding_data);
                if (err_code != NRF_ERROR_NOT_FOUND) {
                    APP_ERROR_CHECK(err_code);

                    // Manipulate identities to exclude peers with no Central Address Resolution.
                    ble_pm_identities_set(PM_PEER_ID_LIST_SKIP_ALL);

                    ble_gap_addr_t * p_peer_addr = &(peer_bonding_data.peer_ble_id.id_addr_info);
                    err_code = ble_advertising_peer_addr_reply(&m_advertising, p_peer_addr);
                    APP_ERROR_CHECK(err_code);
                }
            }
        }
        break; //BLE_ADV_EVT_PEER_ADDR_REQUEST

    default:
        break;
    }
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void) {
    // try to go to system-off mode
    nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
}
