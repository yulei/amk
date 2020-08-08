/**
 * matrix_driver.c
 *  matrix_driver implementation for nrf52
 */

#include "ble_config.h"
#include "matrix_driver.h"
#include "gpio_pin.h"
#include "keyboard.h"
#include "wait.h"
#include "timer.h"
#include "rgb_driver.h"
#include "rgb_effects.h"
#include "rtt.h"

typedef struct {
    matrix_event_callback   event_callback;
    bool                    trigger_mode;
} matrix_driver_t;

matrix_driver_t matrix_driver;

#ifdef MATRIX_USE_TCA6424
#   ifndef MATRIX_DETECT_PIN
#       error "the tca6424 interrupt detect pin must be defined first"
#   endif
#   include "i2c_master.h"
#   include "tca6424.h"
#endif

#ifndef DEBOUNCE
#   define DEBOUNCE 5
#endif

static bool debouncing = false;
static uint16_t debouncing_time = 0;
pin_t row_pins[] = MATRIX_ROW_PINS;
pin_t col_pins[] = MATRIX_COL_PINS;

/* matrix state(1:on, 0:off) */
static matrix_row_t raw_matrix[MATRIX_ROWS];    //raw values
static matrix_row_t matrix[MATRIX_ROWS];        //debounced values


#if defined(MATRIX_USE_GPIO)

#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"

static void matrix_pin_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (action == NRF_GPIOTE_POLARITY_LOTOHI) {
        for (int i = 0; i < MATRIX_ROWS; i++) {
            if (pin == row_pins[i]) {
                if (matrix_driver.event_callback) matrix_driver.event_callback(true);
                break;
            };
        }
    }
}

static void matrix_trigger_start(void)
{
    if (matrix_driver.trigger_mode) {
        return;
    }

    for (int i = 0; i < MATRIX_COLS; i++) {
        nrf_gpio_pin_set(col_pins[i]);
    }

    ret_code_t                 err_code;
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    in_config.pull                       = NRF_GPIO_PIN_PULLDOWN;
    for (uint32_t i = 0; i < MATRIX_ROWS; i++) {
        // NRF_LOG_INFO("trigger init: pin=%d", row_pins[i]);
        err_code = nrf_drv_gpiote_in_init(row_pins[i], &in_config, matrix_pin_event_handler);
        APP_ERROR_CHECK(err_code);
        nrf_drv_gpiote_in_event_enable(row_pins[i], true);
    }
    matrix_driver.trigger_mode = true;
    NRF_LOG_INFO("keyboard matrix trigger mode started");
}

static void matrix_trigger_stop(void)
{
    if (!matrix_driver.trigger_mode) {
        return;
    }

    for (int i = 0; i < MATRIX_COLS; i++) {
        nrf_gpio_pin_clear(col_pins[i]);
    }

    for (uint32_t i = 0; i < MATRIX_ROWS; i++) {
        //NRF_LOG_INFO("trigger uninit: pin=%d", row_pins[i]);
        nrf_drv_gpiote_in_event_disable(row_pins[i]);
        nrf_drv_gpiote_in_uninit(row_pins[i]);
    }
    matrix_driver.trigger_mode = false;
    NRF_LOG_INFO("keyboard matrix trigger mode stopped");
}

static void  init_pins(void)
{
    // don't need pullup/down, since it's pulled down in hardware
    for (int col = 0; col < MATRIX_COLS; col++) {
        gpio_set_output_pushpull(col_pins[col]);
        gpio_write_pin(col_pins[col], 0);
    }
    for (int row = 0; row < MATRIX_ROWS; row++) {
        gpio_set_input_pulldown(row_pins[row]);
    }
}

static void matrix_prepare_sleep(void)
{
    nrf_drv_gpiote_uninit();

    for (uint32_t i = 0; i < NUMBER_OF_PINS; i++) {
        nrf_gpio_cfg_default(i);
    }

    nrf_gpio_cfg_output(CAPS_LED_PIN);
    nrf_gpio_pin_clear(CAPS_LED_PIN);
    
    nrf_gpio_cfg_output(RGBLIGHT_EN_PIN);
    nrf_gpio_pin_clear(RGBLIGHT_EN_PIN);
    
    
    nrf_gpio_cfg_output(27);
    nrf_gpio_pin_clear(27);
    /*nrf_gpio_cfg_output(BATTERY_SAADC_ENABLE_PIN);
    nrf_gpio_pin_clear(BATTERY_SAADC_ENABLE_PIN);
    

    nrf_gpio_cfg_output(19);
    nrf_gpio_pin_clear(19);

    nrf_gpio_cfg_output(20);
    nrf_gpio_pin_clear(20);
    
    nrf_gpio_cfg_output(11);
    nrf_gpio_pin_set(11);

    nrf_gpio_cfg_output(12);
    nrf_gpio_pin_set(12);
    */

    for (uint32_t i = 0; i < MATRIX_COLS; i++) {
        nrf_gpio_cfg_output(col_pins[i]);
        nrf_gpio_pin_set(col_pins[i]);
    }

    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        nrf_gpio_cfg_sense_input(row_pins[i], NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    }

}

static void matrix_scan_start(void) { init_pins(); }
static void matrix_scan_stop(void) { }


#elif defined(MATRIX_USE_TCA6424)

#define GET_PORT(p) (((p) >> 8) & 0xFF)
#define GET_PIN(p) ((p) & 0xFF)

static void set_pin(MATRIX_PIN pin)
{
    uint8_t data = tca6424_read_port(GET_PORT(pin));
    data |= (1<<GET_PIN(pin));
    tca6424_write_port(GET_PORT(pin), data);
}

static void clear_pin(MATRIX_PIN pin)
{
    uint8_t data = tca6424_read_port(GET_PORT(pin));
    data &= ~(1<<GET_PIN(pin));
    tca6424_write_port(GET_PORT(pin), data);
}

static uint8_t read_pin(MATRIX_PIN pin)
{
    uint8_t data = tca6424_read_port(GET_PORT(pin));
    return (data & (1<<GET_PIN(pin))) ? 1 : 0;
}

static void init_pins(void)
{
    tca6424_init();
    // set port0
    tca6424_write_config(TCA6424_PORT0, 0);
    // set port1
    tca6424_write_config(TCA6424_PORT1, 0);
    // set port2
    tca6424_write_config(TCA6424_PORT2, 0xF5);

    // clear output
    tca6424_write_port(TCA6424_PORT0, 0);
    tca6424_write_port(TCA6424_PORT1, 0);
    tca6424_write_port(TCA6424_PORT2, 0);
}

static void matrix_pin_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if ((pin == MATRIX_DETECT_PIN) && (action == NRF_GPIOTE_POLARITY_HITOLO)) {
        // read port will clear the interrupt event
        tca6424_read_port(TCA6424_PORT0);
        tca6424_read_port(TCA6424_PORT1);
        tca6424_read_port(TCA6424_PORT2);
        if (matrix_driver.event_callback) matrix_driver.event_callback(true);
    }
}

static void matrix_trigger_start(matrix_event_callback_f event_cb)
{
    if (matrix_driver.trigger_mode) {
        return;
    }

    for (int i = 0; i < MATRIX_COLS; i++) {
        set_pin(col_pins[i]);
    }

    ret_code_t                 err_code;
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull                       = NRF_GPIO_PIN_PULLUP;
    err_code = nrf_drv_gpiote_in_init(MATRIX_DETECT_PIN, &in_config, matrix_pin_event_handler);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(MATRIX_DETECT_PIN, true);
    matrix_driver.trigger_mode = true;
    matrix_driver.event_callback = event_cb;
    i2c_uninit();
    NRF_LOG_INFO("keyboard matrix trigger mode started");
}

static void matrix_trigger_stop(void)
{
    if (!matrix_driver.trigger_mode) {
        return;
    }

    i2c_init();

    for (int i = 0; i < MATRIX_COLS; i++) {
        clear_pin(col_pins[i]);
    }

    nrf_drv_gpiote_in_event_disable(MATRIX_DETECT_PIN);
    nrf_drv_gpiote_in_uninit(MATRIX_DETECT_PIN);
    matrix_driver.trigger_mode = false;
    matrix_driver.event_callback = NULL;
    NRF_LOG_INFO("keyboard matrix trigger mode stopped");
}

static void matrix_scan_start(void) { init_pins(); }

static void matrix_scan_stop(void) {}

static void matrix_prepare_sleep(void)
{
    i2c_uninit();
    for (uint32_t i = 0; i < MATRIX_COLS; i++) {
        set_pin(col_pins[i]);
    }

    nrf_gpio_cfg_sense_input(MATRIX_DETECT_PIN, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
}

#else
#error "Custom matrix implementation not supported"
#endif
;

__attribute__((weak))
void matrix_init_kb(void){}

void matrix_init(void)
{
    // initialize matrix
    matrix_driver_init();

    // initialize matrix state: all keys off
    memset(&matrix[0], 0, sizeof(matrix));
    memset(&raw_matrix[0], 0, sizeof(raw_matrix));

#ifdef RGB_EFFECTS_ENABLE
    rgb_driver_t* driver = rgb_driver_create(RGB_DRIVER_AW9523B);
    rgb_effects_init(driver);
#endif

    matrix_init_kb();
}

__attribute__((weak))
void matrix_scan_kb(void) {}
uint8_t matrix_scan(void)
{
    bool changed = false;
    for (int col = 0; col < MATRIX_COLS; col++) {
        gpio_write_pin(col_pins[col], 1);
        wait_us(30);

        for(uint8_t row = 0; row < MATRIX_ROWS; row++) {
            matrix_row_t last_row_value    = raw_matrix[row];
            matrix_row_t current_row_value = last_row_value;

            if (gpio_read_pin(row_pins[row])) {
                current_row_value |= (1 << col);
            } else {
                current_row_value &= ~(1 << col);
            }

            if (last_row_value != current_row_value) {
                raw_matrix[row] = current_row_value;
                changed = true;
            }
        }
        gpio_write_pin(col_pins[col], 0);
    }

    if (changed && !debouncing) {
        debouncing = true;
        debouncing_time = timer_read();
    }

    if (debouncing && timer_elapsed(debouncing_time) > DEBOUNCE) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            matrix[row] = raw_matrix[row];
        }
        debouncing = false;
    }

#ifdef RGB_EFFECTS_ENABLE
    rgb_effects_task();
#endif

    matrix_scan_kb();
    return 1;
}

void matrix_driver_init(void)
{
    matrix_driver.event_callback    = NULL;
    matrix_driver.trigger_mode      = false;
    init_pins();
}

void matrix_driver_trigger_start(matrix_event_callback event_cb)
{
    matrix_driver.event_callback    = event_cb;
    matrix_trigger_start();
}

void matrix_driver_trigger_stop(void)
{
    matrix_trigger_stop();
}

void matrix_driver_scan_start(void)
{
    matrix_scan_start();
}

void matrix_driver_scan_stop(void)
{
    matrix_scan_stop();
}

void matrix_driver_prepare_sleep(void)
{
    matrix_prepare_sleep();
}

matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

bool matrix_driver_keys_off(void)
{
    for (int i = 0; i < MATRIX_ROWS; i++) {
        if (matrix[i] != 0) {
            return false;
        }
    }
    return true;
}

// =======================
// tmk hooking
// =======================
void hook_matrix_change(keyevent_t event)
{
    if (!IS_NOEVENT(event)) {
        ble_driver.matrix_changed = 1;
    }
}

