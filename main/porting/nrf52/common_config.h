/**
 * @file common_config.h
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"

#include "app_timer.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define NRF_XSTR(x) #x
#define NRF_NAME(x) NRF_XSTR(x)

#ifndef PRODUCT
    #define PRODUCT                         BLE keyboard
#endif

#ifndef MANUFACTURER
    #define MANUFACTURER                    astro
#endif

#ifndef VBUS_DETECT_PIN
    #define VBUS_DETECT_PIN                 26                                          /**< Default pin for usb sense */
#endif

#ifndef UART_RX_PIN
    #define UART_RX_PIN                     24                                          /**< Default pin for uart rx */
#endif

#ifndef UART_TX_PIN
    #define UART_TX_PIN                     23                                          /**< Default pin for uart tx */
#endif

#ifndef BATTERY_SAADC_ENABLE_PIN
    #define BATTERY_SAADC_ENABLE_PIN        27                                          /**< Enable battery measurement pin */
#endif

#ifndef BATTERY_SAADC_PIN
    #define BATTERY_SAADC_PIN               NRF_SAADC_INPUT_AIN4                        /**< Default pin for saadc */
#endif

#ifdef BLE_PRODUCT
#define DEVICE_NAME                         NRF_NAME(BLE_PRODUCT)                       /**< Name of device. Will be included in the advertising data. */
#else
#define DEVICE_NAME                         NRF_NAME(PRODUCT)                           /**< Name of device. Will be included in the advertising data. */
#endif
#define MANUFACTURER_NAME                   NRF_NAME(MANUFACTURER)                      /**< Manufacturer. Will be passed to Device Information Service. */

#define VENDOR_ID_SOURCE                    0x02                                       /**< Vendor ID Source. */
#ifndef VENDER_ID
    #define VENDER_ID                       0x4154                                     /**< Vendor ID. */
#endif
#ifndef PRODUCT_ID
    #define PRODUCT_ID                      0x00BE                                     /**< Product ID. */
#endif
#ifndef DEVICE_VER
    #define DEVICE_VER                      0x0001                                     /**< Product Version. */
#endif

#define BATTERY_LEVEL_MEAS_SAMPLE           APP_TIMER_TICKS(2)                         /**< Battery level measurement sampling (ticks). */
#define BATTERY_LEVEL_MEAS_INTERVAL         APP_TIMER_TICKS(60*1000)                   /**< Battery level measurement interval (ticks). */

#define VBUS_DETECT_DELAY_INTERVAL          APP_TIMER_TICKS(10)                        /**< VBUS debounce time (ticks). */

#define DEAD_BEEF                           0xDEADBEEF                                 /**< Value used */

#define SCHED_MAX_EVENT_DATA_SIZE           APP_TIMER_SCHED_EVENT_DATA_SIZE            /**< Maximum size of scheduler events. */
#ifdef SVCALL_AS_NORMAL_FUNCTION
#define SCHED_QUEUE_SIZE                    64                                         /**< Maximum number of events in the scheduler queue. More is needed in case of Serialization. */
#else
#define SCHED_QUEUE_SIZE                    64                                         /**< Maximum number of events in the scheduler queue. */
#endif

// battery threshhold
#define BATTERY_LED_THRESHHOLD              20                                         /**< Turn off leds if battery power under this */
#define BATTERY_SHUTDOWN_THRESHHOLD         5                                          /**< Turn off keyboard if battery power under this */

// matrix scanning parameters
#define MAX_SCAN_COUNT                      1000                                       /**< Maximum scan count before turn to trigger mode */
#define KEYBOARD_SCAN_INTERVAL              APP_TIMER_TICKS(10)                        /**< Normal keyboard scan interval */
#define KEYBOARD_SLOW_SCAN_INTERVAL         APP_TIMER_TICKS(100)                       /**< Slow keyboard scan interval */
#define SLEEP_SCAN_OVERFLOW                 (60*100)                                   /**< increase sleep count while scan overlow */
#ifndef SLEEP_COUNT_MAX
#define SLEEP_COUNT_MAX                     10                                         /**< In minutes */
#endif

#define LEDS_NUMBER 0
#define BUTTONS_NUMBER 0

// UART communication
#define SYNC_BYTE_1 0xAA
#define SYNC_BYTE_2 0x55
#define SYNC_PING   0xA5
#define SYNC_PONG   0x5A

typedef enum {
    CMD_KEY_REPORT,
    CMD_MOUSE_REPORT,
    CMD_SYSTEM_REPORT,
    CMD_CONSUMER_REPORT,
    CMD_RESET_TO_BOOTLOADER,
    CMD_SET_LEDS,
    CMD_KEYMAP_SET,
    CMD_KEYMAP_SET_ACK,
    CMD_KEYMAP_GET,
    CMD_KEYMAP_GET_ACK,
    CMD_TOGGLE_SCREEN,
    CMD_TOGGLE_MSC,
} command_t;

#define UART_TX_BUF_SIZE                    128                                        /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                    128                                        /**< UART RX buffer size. */

// reset reason
#define RST_REGISTER                        0x00                                       /**< id of the retain register */
#define RST_BOOTLOADER                      0x01                                       /**< reset to bootloader ? */
#define RST_ERASE_BOND                      0x02                                       /**< reset to erase all bond */
#define RST_USE_BLE                         0x04                                       /**< reset to use ble stack */
#define RST_USE_GZLL                        0x08                                       /**< reset to use gzll stack */

typedef enum {
    NRF_REPORT_ID_KEYBOARD = 1,
    NRF_REPORT_ID_MOUSE,
    NRF_REPORT_ID_SYSTEM,
    NRF_REPORT_ID_CONSUMER,
    NRF_REPORT_ID_COMMAND,
    NRF_REPORT_ID_LAST,
} nrf_report_id;

#define NRF_REPORT_ID_MAX (NRF_REPORT_ID_LAST-NRF_REPORT_ID_KEYBOARD)

#define NRF_INPUT_REPORT_KEYBOARD_MAX_LEN   8
#define NRF_INPUT_REPORT_MOUSE_MAX_LEN      5
#define NRF_INPUT_REPORT_SYSTEM_MAX_LEN     2
#define NRF_INPUT_REPORT_CONSUMER_MAX_LEN   2

#ifndef WDT_ENABLE
#define WDT_ENABLE      1
#endif

#define OUTPUT_RF       0x01
#define OUTPUT_USB      0x02
#define USB_ENABLED     0x80
#define SLEEP_ENABLED   0x40 

typedef struct {
    uint8_t         rf_led;         /**< keyboard led status from ble */
    uint8_t         usb_led;        /**< keyboard led status from usb */
    uint8_t         is_ble;         /**< current is ble stack */
    uint8_t         vbus_enabled;   /**< vbus status */
    uint8_t         output_target;  /**< target of output */
    uint8_t         matrix_changed; /**< matrix has changed */
    uint8_t         battery_power;  /**< power of the battery */
    uint32_t        sleep_count;    /**< count for sleep or not, based on the battery sampling timer */
    uint32_t        scan_count;     /**< idle scan count */
} rf_driver_t;

extern rf_driver_t rf_driver;
typedef void (*rf_send_report_t)(uint8_t type, uint8_t *data, uint8_t size);
typedef void (*rf_prepare_sleep_t)(void);