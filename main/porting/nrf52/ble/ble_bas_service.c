/**
 * @file ble_bas_service.h
 * @brief ble battery service
 */

#include "ble_bas.h"
#include "ble_bas_service.h"

#include "nrf_gpio.h"
#include "nrf_saadc.h"
#include "nrf_drv_saadc.h"

#include "nrf_pwr_mgmt.h"

#define SAADC_SAMPLES 5
static nrf_saadc_value_t m_saadc_buffers[2][SAADC_SAMPLES];

APP_TIMER_DEF(m_battery_sample_timer_id)                             /**< battery voltage sampling timer. */
APP_TIMER_DEF(m_battery_timer_id);                                  /**< Battery timer. */
BLE_BAS_DEF(m_bas);                                                 /**< Structure used to identify the battery service. */

static void battery_level_update(uint8_t level);
static void battery_level_meas_timeout_handler(void* p_context);
static void battery_level_sample_timeout_handler(void* p_context);

static void battery_saadc_init(void);
static void battery_saadc_handler(nrf_drv_saadc_evt_t const * p_event);
static void battery_process_saadc_result(uint32_t result);

void ble_bas_service_init(void) {
    ret_code_t     err_code;
    ble_bas_init_t bas_init_obj;

    memset(&bas_init_obj, 0, sizeof(bas_init_obj));

    bas_init_obj.evt_handler          = NULL;
    bas_init_obj.support_notification = true;
    bas_init_obj.p_report_ref         = NULL;
    bas_init_obj.initial_batt_level   = 100;

    bas_init_obj.bl_rd_sec        = SEC_JUST_WORKS;
    bas_init_obj.bl_cccd_wr_sec   = SEC_JUST_WORKS;
    bas_init_obj.bl_report_rd_sec = SEC_JUST_WORKS;

    err_code = ble_bas_init(&m_bas, &bas_init_obj);
    APP_ERROR_CHECK(err_code);

    // Create battery timer.
    err_code = app_timer_create(&m_battery_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                battery_level_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_battery_sample_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                battery_level_sample_timeout_handler);
    APP_ERROR_CHECK(err_code);

    battery_saadc_init();
}

void ble_bas_service_start(void) {
    ret_code_t     err_code;
    err_code = app_timer_start(m_battery_timer_id, BATTERY_LEVEL_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for performing a battery measurement, and update the Battery Level characteristic in the Battery Service.
 */
static void battery_level_update(uint8_t level) {
    ret_code_t err_code;

    err_code = ble_bas_battery_level_update(&m_bas, level, BLE_CONN_HANDLE_ALL);
    if ((err_code != NRF_SUCCESS) &&
        (err_code != NRF_ERROR_BUSY) &&
        (err_code != NRF_ERROR_RESOURCES) &&
        (err_code != NRF_ERROR_FORBIDDEN) &&
        (err_code != NRF_ERROR_INVALID_STATE) &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
       ) {
        APP_ERROR_HANDLER(err_code);
    }
    NRF_LOG_INFO("battery level update result: %d", err_code);
}


static void battery_level_sample_timeout_handler(void* p_context)
{
    UNUSED_PARAMETER(p_context);
    ret_code_t err_code;
    err_code = nrf_drv_saadc_sample();
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_start(m_battery_sample_timer_id, BATTERY_LEVEL_MEAS_SAMPLE, NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("battery sampling triggered.");
}

static void battery_level_meas_timeout_handler(void * p_context) {
    UNUSED_PARAMETER(p_context);
    if (ble_driver.sleep_count >= SLEEP_COUNT_THRESHHOLD) {
        if (ble_driver.sleep_enabled) {
            NRF_LOG_INFO("Sleep count overflow, goto system off mode");
            nrf_drv_saadc_uninit();
            nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
            return;
        } else {
            ble_driver.sleep_count = 0;
        }
    } else {
        if (!ble_driver.vbus_enabled) {
            ble_driver.sleep_count++;
            NRF_LOG_INFO("Sleep count increased: %d", ble_driver.sleep_count);
        } else {
            ble_driver.sleep_count = 0;
        }
    }
    // turn battery on and kick off sampling timer
    nrf_gpio_pin_set(BATTERY_SAADC_ENABLE_PIN);
    app_timer_start(m_battery_sample_timer_id, BATTERY_LEVEL_MEAS_SAMPLE, NULL);
    NRF_LOG_INFO("battery measurement started.");
}

typedef struct {
    uint32_t value;
    uint32_t percent;
} lipo_val_t;

static const lipo_val_t LIPO_TABLE[] = {
    {4200, 100},
    {4060, 90},
    {3980, 80},
    {3920, 70},
    {3870, 60},
    {3820, 50},
    {3790, 40},
    {3770, 30},
    {3740, 20},
    {3680, 10},
    {3450, 5},
    {3000, 0},
};

static uint32_t compute_percent(uint32_t value, uint32_t index)
{
    NRFX_ASSERT(index>0);

    lipo_val_t cur = LIPO_TABLE[index];
    lipo_val_t prev = LIPO_TABLE[index-1];

    uint32_t percent = cur.percent;
    percent += (value-cur.value)*(prev.percent-cur.percent) / (prev.value-cur.value);
    return percent;
}

static void battery_process_saadc_result(uint32_t result)
{
    // as in PS. result = ((VP-VN) * GAIN) / REFERENCE) * (2^(RESOLUTION - m))
    // VP = INPUT, VN = GND, GAIN = 1/4, REFERENCE = 0.6V, RESOLUTION  = 10 bits, m = 0(single mode)
    // VP = (result/2^10) * (0.6 / 0.25) = (result*2.4)/1024
    // the Battery input was dived by 2 through two resistors
    // battery voltage: vp*2

    uint32_t mv = (result*2400*2)/1024;
    uint32_t percent = 0;
    for (int i = 0; i < sizeof(LIPO_TABLE)/sizeof(LIPO_TABLE[0]); i++) {
        if (mv >= LIPO_TABLE[i].value) {
            if (i==0) {
                percent = 100;
            } else {
                percent = compute_percent(mv, i);
            }
            break;
        }
    }

    NRF_LOG_INFO("battery sampling finished: value=%d, mV=%d, percent=%d.", result, mv, percent);
    ble_driver.battery_power = percent;
    battery_level_update(percent);
}

static void battery_saadc_handler(nrf_drv_saadc_evt_t const * p_event)
{
    NRF_LOG_INFO("battery saadc handler event type: %d", p_event->type);
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE) {
        ret_code_t err_code;

        nrf_saadc_value_t value = 0;
        for (int i = 0; i < p_event->data.done.size; i++) {
            value += p_event->data.done.p_buffer[i];
        }
        value /= p_event->data.done.size;
        if (value < 0) {
            NRF_LOG_INFO("Battery sampling value maybe incorrect: %d", value);
            value = 0;
        }
        battery_process_saadc_result(value);

        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAADC_SAMPLES);
        APP_ERROR_CHECK(err_code);
        // turn off battery and sampling timer
        nrf_gpio_pin_clear(BATTERY_SAADC_ENABLE_PIN);
        app_timer_stop(m_battery_sample_timer_id);
    }
}

static void battery_saadc_init(void)
{
    NRF_LOG_INFO("batter sampling enable pin: %d, ADC pin: %d", BATTERY_SAADC_ENABLE_PIN, BATTERY_SAADC_PIN);

    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(BATTERY_SAADC_PIN);
    channel_config.gain = NRF_SAADC_GAIN1_4;

    err_code = nrf_drv_saadc_init(NULL, battery_saadc_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(0, &channel_config);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_saadc_buffers[0], SAADC_SAMPLES);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_saadc_buffers[1], SAADC_SAMPLES);
    APP_ERROR_CHECK(err_code);

    nrf_gpio_cfg_output(BATTERY_SAADC_ENABLE_PIN);
    nrf_gpio_pin_clear(BATTERY_SAADC_ENABLE_PIN);
}
