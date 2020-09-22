/**
 * @ile gzll_keyboard.c
 */

#include <stdint.h>
#include "gzll_keyboard.h"
#include "nordic_common.h"
#include "app_error.h"
#include "nrf_gzll.h"
#include "nrf_gzll_error.h"
#include "rf_keyboard.h"
#include "usb_interface.h"
#include "report.h"

static uint8_t m_gzll_packet[GZLL_PAYLOAD_SIZE];
static bool m_gzll_host = false;

#define GZLL_BUFFER_ENTRIES     16

typedef struct {
    uint8_t data[GZLL_PAYLOAD_SIZE];
} gzll_entry_t;

typedef struct {
    gzll_entry_t    packets[GZLL_BUFFER_ENTRIES];   /**< Maximum number of entries that can enqueued in the list */
    uint32_t        rp;                             /**< Index to the read location */
    uint32_t        wp;                             /**< Index to the write location */
} gzll_buffer_t;

gzll_buffer_t gzll_packets;

STATIC_ASSERT(sizeof(gzll_buffer_t) % 4 == 0);

static void buffer_init(void)
{
    gzll_packets.rp = 0;
    gzll_packets.wp = 0;
}

static bool buffer_enqueue(uint8_t* report_data, uint8_t report_len)
{
    if (((gzll_packets.wp+1) % GZLL_BUFFER_ENTRIES) == gzll_packets.rp ) {
        // buffer is full
        return false;
    }
    
    gzll_entry_t* element = &gzll_packets.packets[gzll_packets.wp];
    memcpy(&element->data[0], report_data, report_len);
    gzll_packets.wp = (gzll_packets.wp + 1) % GZLL_BUFFER_ENTRIES;
    return true;
}

static bool buffer_dequeue(void)
{
    if (gzll_packets.wp == gzll_packets.rp) {
        // buffer is empty
        return false;
    }

    gzll_entry_t* element = &gzll_packets.packets[gzll_packets.rp];
    memcpy(&m_gzll_packet[0], &element->data[0], GZLL_PAYLOAD_SIZE);
    gzll_packets.rp = (gzll_packets.rp + 1) % GZLL_BUFFER_ENTRIES;
    return true;
}


static void gzll_send_report(uint8_t type, uint8_t *data, uint8_t size);

void gzll_keyboard_init(bool host)
{
    m_gzll_host = host;
    if (m_gzll_host) {
        GAZELLE_ERROR_CODE_CHECK(nrf_gzll_init(NRF_GZLL_MODE_HOST));
        nrf_gzll_set_max_tx_attempts(GZLL_MAX_TX_ATTEMPTS);
        GAZELLE_ERROR_CODE_CHECK(nrf_gzll_enable());
    } else {
        GAZELLE_ERROR_CODE_CHECK(nrf_gzll_init(NRF_GZLL_MODE_DEVICE));
        nrf_gzll_set_max_tx_attempts(GZLL_MAX_TX_ATTEMPTS);
        GAZELLE_ERROR_CODE_CHECK(nrf_gzll_enable());
    }
    rf_keyboard_init(gzll_send_report);
    buffer_init();
}

void gzll_keyboard_start(void)
{
    if (!m_gzll_host) {
    // Add a packet to the TX FIFO to start the data transfer.
    // Next packets to send will be added.
        m_gzll_packet[1] = GZLL_DUMMY_TYPE;
        GAZELLE_ERROR_CODE_CHECK(nrf_gzll_add_packet_to_tx_fifo(GZLL_PIPE_TO_HOST,
                                                                m_gzll_packet,
                                                                GZLL_PAYLOAD_SIZE));
    }
    rf_keyboard_start();
}

void gzll_keyboard_keepalive(void)
{
    if (m_gzll_host)
        return;

    if (nrf_gzll_ok_to_add_packet_to_tx_fifo(GZLL_PIPE_TO_HOST)) {
        m_gzll_packet[1] = GZLL_DUMMY_TYPE;
        GAZELLE_ERROR_CODE_CHECK(nrf_gzll_add_packet_to_tx_fifo(GZLL_PIPE_TO_HOST,
                                                                m_gzll_packet,
                                                                GZLL_PAYLOAD_SIZE));
    }
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

    if (tx_info.payload_received_in_ack) {
        // if ack was sent with payload, pop them from rx fifo.
        GAZELLE_ERROR_CODE_CHECK(nrf_gzll_fetch_packet_from_rx_fifo(pipe, dummy, &dummy_length));
        rf_driver.usb_led = dummy[1];
    }
    if (buffer_dequeue()) {
        GAZELLE_ERROR_CODE_CHECK(nrf_gzll_add_packet_to_tx_fifo(pipe, m_gzll_packet, GZLL_PAYLOAD_SIZE));
    }
}


/**@brief Callback function for Gazell Transmit fail. Resends the current packet.
 */
void nrf_gzll_device_tx_failed(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
    uint8_t  dummy[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];
    uint32_t dummy_length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;

    // If the transmission failed, send a new packet.
    if (tx_info.payload_received_in_ack) {
        // if ack was sent with payload, pop them from rx fifo.
        GAZELLE_ERROR_CODE_CHECK(nrf_gzll_fetch_packet_from_rx_fifo(pipe, dummy, &dummy_length));
    }

    GAZELLE_ERROR_CODE_CHECK(nrf_gzll_add_packet_to_tx_fifo(pipe, m_gzll_packet, GZLL_PAYLOAD_SIZE));
}


/**@brief Callback function for Gazell Receive Data Ready. Flushes the receive's FIFO.
 */
void nrf_gzll_host_rx_data_ready(uint32_t pipe, nrf_gzll_host_rx_info_t rx_info)
{
    if (!m_gzll_host) {
        return;
    }

    uint8_t  dummy[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];
    uint32_t dummy_length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;

    // Pop packet and write first byte of the payload to the GPIO port.
    bool result_value = nrf_gzll_fetch_packet_from_rx_fifo(pipe, dummy, &dummy_length);

    if (!result_value) {
        NRF_LOG_ERROR("HOST RX fifo error ");
    }

    if (dummy_length> 0) {
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
        default:
            NRF_LOG_INFO("GZLL HOST: unknown report type: %d", dummy[1]);
            break;
        }
    }

    dummy[1] = rf_driver.usb_led;

    result_value = nrf_gzll_add_packet_to_tx_fifo(pipe, dummy, dummy_length);
    if (!result_value) {
        NRF_LOG_ERROR("HOST TX fifo error ");
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

    uint8_t packet[GZLL_PAYLOAD_SIZE];

    switch (type) {
    case NRF_REPORT_ID_KEYBOARD:
        memcpy(packet, data, size);
        break;
    case NRF_REPORT_ID_MOUSE:
        memcpy(&packet[2], data, size);
        break;
    case NRF_REPORT_ID_SYSTEM:
    case NRF_REPORT_ID_CONSUMER:
        memcpy(&packet[2], data, size);
        break;
    default:
        NRF_LOG_INFO("GZLL: unknown report type: %d", type);
        return;
    }
    packet[1] = type;
    if (nrf_gzll_ok_to_add_packet_to_tx_fifo(GZLL_PIPE_TO_HOST)) {
        memcpy(m_gzll_packet, packet, GZLL_PAYLOAD_SIZE);
        GAZELLE_ERROR_CODE_CHECK(nrf_gzll_add_packet_to_tx_fifo(GZLL_PIPE_TO_HOST, m_gzll_packet, GZLL_PAYLOAD_SIZE));
    } else {
        if (!buffer_enqueue(packet, GZLL_PAYLOAD_SIZE)) {
            NRF_LOG_WARNING("GZLL: buffer queue full");
        }
    }
}