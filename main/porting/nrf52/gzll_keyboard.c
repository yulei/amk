/**
 * @ile gzll_keyboard.c
 */

#include <stdint.h>
#include "gzll_keyboard.h"
#include "nordic_common.h"
#include "app_error.h"
#include "nrf_gzll.h"
#include "nrf_gzll_error.h"
#include "nrf_drv_clock.h"
#include "eeconfig_flash.h"
#include "rf_keyboard.h"
#include "usb_interface.h"
#include "report.h"

static uint8_t m_gzll_packet[GZLL_PAYLOAD_SIZE];
static uint8_t m_gzll_keepalive[GZLL_PAYLOAD_SIZE];
static bool m_gzll_host = false;

static void gzll_send_report(uint8_t type, uint8_t *data, uint8_t size);
static void gzll_prepare_sleep(void);
static void gzll_keyboard_keepalive(void);

void gzll_keyboard_init(bool host)
{
    m_gzll_host = host;
    memset(m_gzll_packet, 0, sizeof(m_gzll_packet));
    memset(m_gzll_keepalive, 0, sizeof(m_gzll_keepalive));
    if (!nrf_drv_clock_init_check()) {
        ret_code_t err_code = nrf_drv_clock_init();
        APP_ERROR_CHECK(err_code);
    }
    nrf_drv_clock_lfclk_request(NULL);
    
    if (m_gzll_host) {
        NRF_LOG_INFO("GZLL init to host");
        GAZELLE_ERROR_CODE_CHECK(nrf_gzll_init(NRF_GZLL_MODE_HOST));
    } else {
        NRF_LOG_INFO("GZLL init to device");
        GAZELLE_ERROR_CODE_CHECK(nrf_gzll_init(NRF_GZLL_MODE_DEVICE));
    }

    nrf_gzll_set_max_tx_attempts(GZLL_MAX_TX_ATTEMPTS);
    nrf_gzll_set_base_address_0(GZLL_BASE_ADDRESS_0);
    nrf_gzll_set_base_address_1(GZLL_BASE_ADDRESS_1);

#ifdef EECONFIG_FLASH
    fds_eeprom_init();
#endif

    NRF_LOG_INFO("Start to init rf keyboard");
    rf_keyboard_init(gzll_send_report, gzll_prepare_sleep);
}

void gzll_keyboard_start(void)
{
    GAZELLE_ERROR_CODE_CHECK(nrf_gzll_enable());
    gzll_keyboard_keepalive();
    rf_keyboard_start();
}

void gzll_keyboard_stop(void)
{
}

void gzll_prepare_sleep(void)
{
    // Disable gazell.
    nrf_gzll_disable();

    // Wait for Gazell to shut down.
    while (nrf_gzll_is_enabled()) { }

    // Clean up after Gazell.
    NVIC_DisableIRQ(RADIO_IRQn);
    NVIC_DisableIRQ(NRF_GZLL_TIMER_IRQn);
    NVIC_DisableIRQ(NRF_GZLL_SWI_IRQn);
    NVIC_ClearPendingIRQ(RADIO_IRQn);
    NVIC_ClearPendingIRQ(NRF_GZLL_TIMER_IRQn);
    NVIC_ClearPendingIRQ(NRF_GZLL_SWI_IRQn);
}

void gzll_keyboard_keepalive(void)
{
    if (m_gzll_host)
        return;

    if (nrf_gzll_ok_to_add_packet_to_tx_fifo(GZLL_PIPE_TO_HOST)) {
        m_gzll_keepalive[1] = GZLL_KEEPALIVE;
        if (!nrf_gzll_add_packet_to_tx_fifo(GZLL_PIPE_TO_HOST, m_gzll_keepalive, GZLL_PAYLOAD_SIZE)) {
            NRF_LOG_ERROR("GZLL failed add to tx fifo, code:%d", nrf_gzll_get_error_code());
        }
    }
    NRF_LOG_INFO("GZLL keep alive packet sent");
}

/**@brief Callback function for Gazell Disabled.
 */
void nrf_gzll_disabled()
{
}

/**@brief Callback function for Gazell Transmit Success. Adds new packet to tx fifo.
 */
void nrf_gzll_device_tx_success(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
    uint8_t    dummy[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];
    uint32_t   dummy_length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;

    NRF_LOG_INFO("GZLL tx success");
    if (tx_info.payload_received_in_ack) {
        // if ack was sent with payload, pop them from rx fifo.
        if (!nrf_gzll_fetch_packet_from_rx_fifo(pipe, dummy, &dummy_length)) {
            NRF_LOG_ERROR("GZLL failed fetch from rx fifo, error:%d", nrf_gzll_get_error_code());
        } else {
            // update led status from host ack
            rf_driver.rf_led = dummy[1];
            NRF_LOG_INFO("GZLL rf led: %d", dummy[1]);
            //gzll_keyboard_keepalive();
        }
    }
}


/**@brief Callback function for Gazell Transmit fail. Resends the current packet.
 */
void nrf_gzll_device_tx_failed(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
    uint8_t  dummy[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];
    uint32_t dummy_length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;

    NRF_LOG_INFO("GZLL tx failed on pipe:%d", pipe);
    if (tx_info.payload_received_in_ack) {
        // if ack was sent with payload, pop them from rx fifo.
        if (!nrf_gzll_fetch_packet_from_rx_fifo(pipe, dummy, &dummy_length)) {
            NRF_LOG_ERROR("GZLL failed fetch from rx fifo, error:%d", nrf_gzll_get_error_code());
        }
    }

    // If the transmission failed, re-send current packet.
    if (!nrf_gzll_add_packet_to_tx_fifo(pipe, m_gzll_packet, GZLL_PAYLOAD_SIZE)) {
        NRF_LOG_ERROR("GZLL failed to readded packet to tx fifo, error:%d", nrf_gzll_get_error_code());
    }
}


/**@brief Callback function for Gazell Receive Data Ready. Flushes the receive's FIFO.
 */
void nrf_gzll_host_rx_data_ready(uint32_t pipe, nrf_gzll_host_rx_info_t rx_info)
{
    if (!m_gzll_host) {
        return;
    }

    NRF_LOG_INFO("GZLL HOST data received");
    uint8_t  dummy[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];
    uint32_t dummy_length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;

    // Pop packet and write first byte of the payload to the GPIO port.
    bool result_value = nrf_gzll_fetch_packet_from_rx_fifo(pipe, dummy, &dummy_length);

    if (!result_value) {
        NRF_LOG_ERROR("HOST RX fifo error: %d", nrf_gzll_get_error_code());
    }

    if (dummy_length > 1) {
        switch (dummy[1]) {
        case NRF_REPORT_ID_KEYBOARD: {
            dummy[1] = 0;
            report_keyboard_t report;
            memcpy(&report, dummy, sizeof(report));
            nrf_usb_send_report(NRF_REPORT_ID_KEYBOARD, &report, sizeof(report));
        } break;
        case NRF_REPORT_ID_MOUSE: {
            report_mouse_t report;
            memcpy(&report, &dummy[2], sizeof(report));
            nrf_usb_send_report(NRF_REPORT_ID_MOUSE, &report, sizeof(report));
        } break;
        case NRF_REPORT_ID_SYSTEM: {
            uint16_t report;
            memcpy(&report, &dummy[2], sizeof(report));
            nrf_usb_send_report(NRF_REPORT_ID_SYSTEM, &report, sizeof(report));
        } break;
        case NRF_REPORT_ID_CONSUMER: {
            uint16_t report;
            memcpy(&report, &dummy[2], sizeof(report));
            nrf_usb_send_report(NRF_REPORT_ID_CONSUMER, &report, sizeof(report));
        } break;
        case GZLL_KEEPALIVE:
            // keep alive packet
            break;
        default:
            NRF_LOG_INFO("GZLL HOST: unknown report type: %d", dummy[1]);
            break;
        }
    }

    dummy[1] = rf_driver.usb_led;

    result_value = nrf_gzll_add_packet_to_tx_fifo(pipe, dummy, dummy_length);
    if (!result_value) {
        NRF_LOG_ERROR("HOST TX fifo error:%d", nrf_gzll_get_error_code());
    }
}

static void gzll_send_report(uint8_t type, uint8_t* data, uint8_t size)
{
    if (m_gzll_host) {
        // do nothing
        return;
    }

    if (size > GZLL_PAYLOAD_SIZE) {
        NRF_LOG_WARNING("GZLL: invalid report size: %d", size);
        return;
    }

    m_gzll_packet[1] = GZLL_KEEPALIVE;
    switch (type) {
    case NRF_REPORT_ID_KEYBOARD:
        memcpy(m_gzll_packet, data, size);
        m_gzll_packet[1] = type;
        break;
    case NRF_REPORT_ID_MOUSE:
        memcpy(&m_gzll_packet[2], data, size);
        m_gzll_packet[1] = type;
        break;
    case NRF_REPORT_ID_SYSTEM:
    case NRF_REPORT_ID_CONSUMER:
        memcpy(&m_gzll_packet[2], data, size);
        m_gzll_packet[1] = type;
        break;
    default:
        NRF_LOG_INFO("GZLL: unknown report type: %d", type);
        return;
    }
    if (!nrf_gzll_add_packet_to_tx_fifo(GZLL_PIPE_TO_HOST, m_gzll_packet, GZLL_PAYLOAD_SIZE)) {
        NRF_LOG_ERROR("GZLL: failed to add packet to tx fifo, code:%d", nrf_gzll_get_error_code());
    }
}