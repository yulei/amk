/**
 * usb_backend_uart.h
 *      send usb report throught the uart 
 */

#include "usb_interface.h"
#include "app_uart.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_uart.h"

typedef struct {
    nrf_usb_event_handler_t event;
    bool vbus_enabled;
    bool uart_enabled;
} nrf_usb_config_t;

static nrf_usb_config_t usb_config;
static void vbus_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
static void usb_update_state();
static void uart_init(void);
static void uart_uninit(void);
static void uart_event_handle(app_uart_evt_t * p_event);
static void uart_send_cmd(command_t cmd, const uint8_t* report, uint32_t size);
static uint8_t compute_checksum(const uint8_t* data, uint32_t size);

void nrf_usb_init(nrf_usb_event_handler_t* eh)
{
    usb_config.event.enable_cb  = eh->enable_cb;
    usb_config.event.disable_cb = eh->disable_cb;
    usb_config.event.suspend_cb = eh->suspend_cb;
    usb_config.event.resume_cb  = eh->resume_cb;
    usb_config.event.leds_cb    = eh->leds_cb;

    ret_code_t err_code;
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    err_code = nrf_drv_gpiote_in_init(VBUS_DETECT_PIN, &in_config, vbus_event_handler);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(VBUS_DETECT_PIN, true);

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
    usb_config.vbus_enabled = nrf_drv_gpiote_in_is_set(VBUS_DETECT_PIN) ? true : false;

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

static void vbus_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
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
        case APP_UART_DATA_READY:
            break;
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

static void uart_send_cmd(command_t cmd, const uint8_t* report, uint32_t size)
{
    if (!usb_config.vbus_enabled) {
        NRF_LOG_INFO("No VBUS power, can send command through UART");
        return;
    }
    if (!usb_config.uart_enabled) {
        uart_init();
    }

    uint8_t checksum = cmd;
    checksum += compute_checksum(report, size);
    app_uart_put(SYNC_BYTE_1);
    app_uart_put(SYNC_BYTE_2);
    app_uart_put(size+3);
    app_uart_put(checksum);
    app_uart_put(cmd);
    for (uint32_t i = 0; i < size; i++) {
        app_uart_put(report[i]);
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