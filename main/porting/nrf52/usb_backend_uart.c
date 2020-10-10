/**
 * usb_backend_uart.h
 *      send usb report throught the uart 
 */

#include "usb_interface.h"
#include "amk_keymap.h"
#include "app_uart.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "nrf_uart.h"
#include "nrf_delay.h"

typedef struct {
    nrf_usb_event_handler_t event;
    bool vbus_enabled;
    bool uart_enabled;
} nrf_usb_config_t;

#define NRF_RECV_BUF_SIZE   64
typedef struct {
    uint8_t data[NRF_RECV_BUF_SIZE];
    uint32_t count;
} nrf_usb_buf_t;

static nrf_usb_config_t usb_config;
static nrf_usb_buf_t usb_buffer;

static void vbus_event_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
static void usb_update_state();
static void uart_init(void);
static void uart_uninit(void);
static void uart_event_handle(app_uart_evt_t * p_event);
static void uart_send_cmd(command_t cmd, const uint8_t* report, uint32_t size);
static void uart_process_data(uint8_t data);
static void uart_process_cmd(const uint8_t* cmd, uint32_t size);
static uint8_t compute_checksum(const uint8_t* data, uint32_t size);

void nrf_usb_init(nrf_usb_event_handler_t* eh)
{
    usb_config.event.enable_cb  = eh->enable_cb;
    usb_config.event.disable_cb = eh->disable_cb;
    usb_config.event.suspend_cb = eh->suspend_cb;
    usb_config.event.resume_cb  = eh->resume_cb;
    usb_config.event.leds_cb    = eh->leds_cb;

    for(int i = 0; i < NRF_RECV_BUF_SIZE; i++) {
        usb_buffer.data[i] = 0;
    }
    usb_buffer.count = 0;

    ret_code_t err_code;
    nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    err_code = nrfx_gpiote_in_init(VBUS_DETECT_PIN, &in_config, vbus_event_handler);
    APP_ERROR_CHECK(err_code);
    nrfx_gpiote_in_event_enable(VBUS_DETECT_PIN, true);

    usb_update_state();
}

void nrf_usb_send_report(nrf_report_id report, const void *data, size_t size)
{
    switch(report) {
        case NRF_REPORT_ID_KEYBOARD: 
            uart_send_cmd(CMD_KEY_REPORT, (uint8_t*)data, size);
            NRF_LOG_INFO("Key report:[%x%x]", ((uint32_t*)data)[0], ((uint32_t*)data)[1]);
            break;
#ifdef MOUSEKEY_ENABLE
        case NRF_REPORT_ID_MOUSE:
            uart_send_cmd(CMD_MOUSE_REPORT, (uint8_t*)data, size);
            NRF_LOG_INFO("Mouse report:");
            for (int i = 0; i < size; i++) {
                NRF_LOG_INFO("0x%x", ((uint8_t*)data)[i]);
            }
            break;
#endif
#ifdef EXTRAKEY_ENABLE
        case NRF_REPORT_ID_SYSTEM:
            uart_send_cmd(CMD_SYSTEM_REPORT, (uint8_t*)data, size);
            NRF_LOG_INFO("system report: 0x%x", *((uint32_t*)data));
            break;
        case NRF_REPORT_ID_CONSUMER:
            uart_send_cmd(CMD_CONSUMER_REPORT, (uint8_t*)data, size);
            NRF_LOG_INFO("consumer report: 0x%x", *((uint32_t*)data));
            break;
#endif
        default:
            NRF_LOG_INFO("Unknow report id: %d", report);
            break;
    }
}

void nrf_usb_wakeup(void) {}

void nrf_usb_prepare_sleep(void)
{
    nrf_gpio_cfg_sense_input(VBUS_DETECT_PIN, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    nrf_delay_ms(1);
}

void nrf_usb_reboot(void)
{
    if (!usb_config.uart_enabled) {
        NRF_LOG_INFO("UART not enabled, can't send reboot command");
        return;
    }

    uint8_t checksum = CMD_RESET_TO_BOOTLOADER;
    app_uart_put(SYNC_BYTE_1);
    app_uart_put(SYNC_BYTE_2);
    app_uart_put(3);
    app_uart_put(checksum);
    app_uart_put(CMD_RESET_TO_BOOTLOADER);
    NRF_LOG_INFO("send reboot command to usb controller, set output to ble");
}

static void usb_update_state()
{
    usb_config.vbus_enabled = nrfx_gpiote_in_is_set(VBUS_DETECT_PIN) ? true : false;

    if (usb_config.vbus_enabled) {
        uart_init();
        usb_config.event.enable_cb();
        NRF_LOG_INFO("VBUS on, turn on uart");
    } else {
        usb_config.event.disable_cb();
        uart_uninit();
        NRF_LOG_INFO("VBUS off, turn off uart");
    }
}

static void vbus_event_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    NRF_LOG_INFO("PIN EVENT: pin=%d, action=%d", pin, action);
    if (pin == VBUS_DETECT_PIN) {
        usb_update_state();
    }
}

static void uart_event_handle(app_uart_evt_t *p_event)
{
    switch (p_event->evt_type) {
        case APP_UART_COMMUNICATION_ERROR:
            NRF_LOG_INFO("UART communication error: 0x%x", p_event->data.error_communication);
            break;
        case APP_UART_FIFO_ERROR:
            app_uart_flush();
            break;
        case APP_UART_DATA_READY: {
                uint8_t d = 0;
                if (NRF_SUCCESS == app_uart_get(&d)) {
                    uart_process_data(d);
                }
            } break;
        case APP_UART_TX_EMPTY:
            break;
        default:
            break;
    }
}

static void uart_init(void)
{
    uint32_t err_code;
    if (usb_config.uart_enabled) {
        NRF_LOG_INFO("uart already enabled.\n");
        return;
    }

    NRF_LOG_INFO("uart tx pin: %d, rx pin: %d", UART_TX_PIN, UART_RX_PIN);
    const app_uart_comm_params_t comm_params = {
        .rx_pin_no = UART_RX_PIN,//NRF_UART_PSEL_DISCONNECTED,//UART_RX_PIN,
        .tx_pin_no = UART_TX_PIN,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity = false,
        .baud_rate = NRF_UART_BAUDRATE_115200,
    };

    APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_event_handle,
                         APP_IRQ_PRIORITY_LOWEST,
                         err_code);

    APP_ERROR_CHECK(err_code);
    usb_config.uart_enabled = true;
    NRF_LOG_INFO("uart enabled.\n");
}

static void uart_uninit(void)
{
    uint32_t err_code;
    if (!usb_config.uart_enabled) {
        NRF_LOG_INFO("uart not enabled.\n");
        return;
    }
    err_code = app_uart_close();
    APP_ERROR_CHECK(err_code);
    usb_config.uart_enabled = false;
    NRF_LOG_INFO("uart disabled.\n");
}

static void uart_send_cmd(command_t cmd, const uint8_t* data, uint32_t size)
{
    if (!usb_config.vbus_enabled) {
        NRF_LOG_INFO("No VBUS power, can send command through UART");
        return;
    }
    if (!usb_config.uart_enabled) {
        uart_init();
    }

    uint8_t checksum = cmd;
    checksum += compute_checksum(data, size);
    app_uart_put(SYNC_BYTE_1);
    app_uart_put(SYNC_BYTE_2);
    app_uart_put(size+3);
    app_uart_put(checksum);
    app_uart_put(cmd);
    for (uint32_t i = 0; i < size; i++) {
        app_uart_put(data[i]);
    }
}

static void uart_process_data(uint8_t data)
{
    switch(usb_buffer.count) {
    case 0:
    // first byte
        if (data == SYNC_BYTE_1) {
            usb_buffer.data[usb_buffer.count++] = data;
        } else {
            NRF_LOG_WARNING("Invalid sync 1 received");
        }
        break;
    case 1:
    // second byte
        if (data == SYNC_BYTE_2) {
            usb_buffer.data[usb_buffer.count++] = data;
        } else {
            NRF_LOG_WARNING("Invalid sync 2 received");
            usb_buffer.count = 0;
        }
        break;
    default:
        usb_buffer.data[usb_buffer.count++] = data;
        if ((usb_buffer.count > 2) && (usb_buffer.count == (usb_buffer.data[2] + 2))) {
            // full packet received
            uint8_t* cmd = &usb_buffer.data[2];
            uint8_t checksum = compute_checksum(cmd + 2, cmd[0] - 2);
            if (checksum != cmd[1]) {
                // invalid checksum
                NRF_LOG_WARNING("Checksum mismatch: SRC:%x, CUR:%x", cmd[1], checksum);
            } else {
                uart_process_cmd(&cmd[2], cmd[0]-2);
            }
            usb_buffer.count = 0;
        }
        break;
    }
}

static void uart_process_cmd(const uint8_t* cmd, uint32_t size)
{
    switch(cmd[0]) {
        case CMD_SET_LEDS: {
            NRF_LOG_INFO("Led setting from usb master: %x", cmd[1]);
            usb_config.event.leds_cb(cmd[1]);
        } break;
        case CMD_KEYMAP_SET: {
            NRF_LOG_INFO("Keymap Set: layer=%d, row=%d, col=%d, key=%d", cmd[1], cmd[2], cmd[3], cmd[4]);
            amk_keymap_set(cmd[1], cmd[2], cmd[3], cmd[4]);
        } break;
        case CMD_KEYMAP_GET: {
            uint16_t keycode = amk_keymap_get(cmd[1], cmd[2], cmd[3]);
            NRF_LOG_INFO("Keymap Get: layer=%d, row=%d, col=%d, key=%d", cmd[1], cmd[2], cmd[3], keycode);
            uart_send_cmd(CMD_KEYMAP_GET_ACK, (uint8_t*)(&keycode), 2);
        } break;
        default: {
            // invalid command
            NRF_LOG_WARNING("Invalid command from usb master: %x", cmd[0]);
        } break;
    }
}

static uint8_t compute_checksum(const uint8_t* data, uint32_t size)
{
    uint8_t checksum = 0;
    for (uint32_t i = 0; i < size; i++) {
        checksum += data[i];
    }
    return checksum;
}