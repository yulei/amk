/**
 * @file ble_config.h
 * @brief commond configuration for the ble
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "ble_srv_common.h"
#include "peer_manager.h"
#include "app_timer.h"

#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"

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

#define DEVICE_NAME                         NRF_NAME(PRODUCT)                           /**< Name of device. Will be included in the advertising data. */
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

#define DEFAULT_TX_POWER_LEVEL              4                                          /**< The default tx power level */
#define APP_ADV_FAST_INTERVAL               0x0028                                     /**< Fast advertising interval (in units of 0.625 ms. This value corresponds to 25 ms.). */
#define APP_ADV_SLOW_INTERVAL               0x0C80                                     /**< Slow advertising interval (in units of 0.625 ms. This value corrsponds to 2 seconds). */

#define APP_ADV_FAST_DURATION               3000                                       /**< The advertising duration of fast advertising in units of 10 milliseconds. */
#define APP_ADV_SLOW_DURATION               18000                                      /**< The advertising duration of slow advertising in units of 10 milliseconds. */

#define APP_BLE_OBSERVER_PRIO               3                                          /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG                1                                          /**< A tag identifying the SoftDevice BLE configuration. */


#define BATTERY_LEVEL_MEAS_SAMPLE           APP_TIMER_TICKS(2)                         /**< Battery level measurement sampling (ticks). */
#define BATTERY_LEVEL_MEAS_INTERVAL         APP_TIMER_TICKS(60*1000)                   /**< Battery level measurement interval (ticks). */

// sleep count threadhold
#define SLEEP_COUNT_THRESHHOLD              10                                         /**< In minutes */

/*lint -emacro(524, MIN_CONN_INTERVAL) // Loss of precision */
#define MIN_CONN_INTERVAL                   MSEC_TO_UNITS(7.5, UNIT_1_25_MS)           /**< Minimum connection interval (7.5 ms) */
#define MAX_CONN_INTERVAL                   MSEC_TO_UNITS(30, UNIT_1_25_MS)            /**< Maximum connection interval (30 ms). */
#define SLAVE_LATENCY                       6                                          /**< Slave latency. */
#define CONN_SUP_TIMEOUT                    MSEC_TO_UNITS(1000, UNIT_10_MS)            /**< Connection supervisory timeout (1000 ms). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY      APP_TIMER_TICKS(5000)                      /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY       APP_TIMER_TICKS(30000)                     /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT        3                                          /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                      1                                          /**< Perform bonding. */
#define SEC_PARAM_MITM                      0                                          /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                      0                                          /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS                  0                                          /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES           BLE_GAP_IO_CAPS_NONE                       /**< No I/O capabilities. */
#define SEC_PARAM_OOB                       0                                          /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE              7                                          /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE              16                                         /**< Maximum encryption key size. */

#define OUTPUT_REPORT_INDEX                 0                                          /**< Index of Output Report. */
#define OUTPUT_REPORT_MAX_LEN               1                                          /**< Maximum length of Output Report. */
#define OUTPUT_REP_REF_ID                   0                                          /**< Id of reference to Keyboard Output Report. */

#define FEATURE_REPORT_INDEX                0                                          /**< Index of Feature Report. */
#define FEATURE_REPORT_MAX_LEN              2                                          /**< Maximum length of Feature Report. */
#define FEATURE_REP_REF_ID                  0                                          /**< ID of reference to Keyboard Feature Report. */

#define MAX_BUFFER_ENTRIES                  6                                          /**< Number of elements that can be enqueued */

#define BASE_USB_HID_SPEC_VERSION           0x0101                                     /**< Version number of base USB HID Specification implemented by this application. */

#define DEAD_BEEF                           0xDEADBEEF                                 /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define SCHED_MAX_EVENT_DATA_SIZE           APP_TIMER_SCHED_EVENT_DATA_SIZE            /**< Maximum size of scheduler events. */
#ifdef SVCALL_AS_NORMAL_FUNCTION
#define SCHED_QUEUE_SIZE                    20                                         /**< Maximum number of events in the scheduler queue. More is needed in case of Serialization. */
#else
#define SCHED_QUEUE_SIZE                    10                                         /**< Maximum number of events in the scheduler queue. */
#endif

// battery threshhold
#define BATTERY_LED_THRESHHOLD              20                                         /**< Turn off leds if battery power under this */
#define BATTERY_SHUTDOWN_THRESHHOLD         5                                          /**< Turn off keyboard if battery power under this */

// matrix scanning parameters
#define MAX_SCAN_COUNT                      1000                                       /**< Maximum scan count before turn to trigger mode */
#define KEYBOARD_SCAN_INTERVAL              APP_TIMER_TICKS(10)                        /**< Normal keyboard scan interval */
#define KEYBOARD_SLOW_SCAN_INTERVAL         APP_TIMER_TICKS(100)                       /**< Slow keyboard scan interval */

#define LEDS_NUMBER 0
#define BUTTONS_NUMBER 0

// UART communication
#define SYNC_BYTE_1 0xAA
#define SYNC_BYTE_2 0x55

typedef enum {
  CMD_KEY_REPORT,
  CMD_MOUSE_REPORT,
  CMD_SYSTEM_REPORT,
  CMD_CONSUMER_REPORT,
  CMD_RESET_TO_BOOTLOADER,
  CMD_SET_LEDS,
} command_t;

#define UART_TX_BUF_SIZE                    128                                        /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                    128                                        /**< UART RX buffer size. */

// reset reason
#define RST_REGISTER                        0x00                                       /**< id of the retain register */
#define RST_BOOTLOADER                      0x01                                       /**< reset to bootloader ? */
#define RST_ERASE_BOND                      0x02                                       /**< reset to erase all bond */

typedef enum {
    NRF_REPORT_ID_KEYBOARD = 1,

#ifdef MOUSEKEY_ENABLE
    NRF_REPORT_ID_MOUSE,
#endif
#ifdef EXTRAKEY_ENABLE
    NRF_REPORT_ID_SYSTEM,
    NRF_REPORT_ID_CONSUMER,
#endif
    NRF_REPORT_ID_LAST,
} nrf_report_id;

#define NRF_REPORT_ID_MAX (NRF_REPORT_ID_LAST-NRF_REPORT_ID_KEYBOARD)

#define NRF_INPUT_REPORT_KEYBOARD_MAX_LEN 8
#ifdef MOUSEKEY_ENABLE
#define NRF_INPUT_REPORT_MOUSE_MAX_LEN 5
#endif

#ifdef EXTRAKEY_ENABLE
#define NRF_INPUT_REPORT_SYSTEM_MAX_LEN 2
#define NRF_INPUT_REPORT_CONSUMER_MAX_LEN 2
#endif

#define OUTPUT_BLE      0x01
#define OUTPUT_USB      0x02

typedef struct {
    pm_peer_id_t    peer_id;        /**< Device reference handle to the current bonded central. */
    uint16_t        conn_handle;    /**< Handle of the current connection. */

    uint8_t         ble_led;        /**< keyboard led status from ble */
    uint8_t         usb_led;        /**< keyboard led status from usb */
    uint8_t         vbus_enabled;   /**< vbus status */
    uint8_t         trigger_enabled;/**< enable trigger mode or not */     
    uint8_t         output_target;  /**< target of output */
    uint8_t         matrix_changed; /**< matrix has changed */
    uint8_t         battery_power;  /**< power of the battery */
    uint32_t        sleep_count;    /**< count for sleep or not, based on the battery sampling timer */
    uint32_t        scan_count;     /**< idle scan count */
} ble_driver_t;

extern ble_driver_t ble_driver;
