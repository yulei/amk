/**
 * @file is31fl3193.c
 * @brief driver implementation for is31fl3193
 */

#include <string.h>
#include "is31fl3193.h"
#include "rgb_common.h"
#include "i2c.h"
#include "wait.h"
#include "amk_printf.h"

#ifndef FL3193_DEBUG
#define FL3193_DEBUG 1
#endif

#if FL3193_DEBUG
#define fl3193_debug  amk_printf
#else
#define fl3193_debug(...)
#endif

#define PWM_BUFFER_SIZE     3

#define SHUTDOWN_REG        0x00
#define BREATH_REG          0x01
#define LED_MODE_REG        0x02
#define CURRENT_REG         0x03

#define PWM_REG             0x04
#define UPDATE_REG          0x07
#define CONTROL_REG         0x1D
#define RESET_REG           0x2F

#ifndef IS31FL3193_NUM
    #define IS31FL3193_NUM  1
#endif

#ifndef IS31FL3193_CURRENT
#define IS31FL3193_CURRENT  0x08
#endif

#define TIMEOUT             100
#ifndef IS31FL3193_I2C_ID
#define IS31FL3193_I2C_ID     I2C_INSTANCE_1
#endif

static i2c_handle_t i2c_inst;

typedef struct {
    i2c_led_t       i2c_led;
    uint8_t         pwm_buffer[PWM_BUFFER_SIZE + 1];
    bool            pwm_dirty;
    bool            ready;
} is31fl3193_driver_t;

static is31fl3193_driver_t is31_drivers[IS31FL3193_NUM] = {0};
static bool fl3193_available[IS31FL3193_NUM] = {false};

static bool init_driver(is31fl3193_driver_t *driver);
static void uninit_driver(i2c_led_t *driver);

bool is31fl3193_available(uint8_t index)
{
    return fl3193_available[index];
}

i2c_led_t *is31fl3193_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num)
{
    is31fl3193_driver_t *driver = &is31_drivers[index];
    if (driver->ready) return &driver->i2c_led;

    driver->i2c_led.addr        = addr;
    driver->i2c_led.index       = index;
    driver->i2c_led.led_start   = led_start;
    driver->i2c_led.led_num     = led_num;
    driver->i2c_led.data        = driver;

    driver->ready = init_driver(driver);

    fl3193_debug("IS31FL3193: initialized: addr=0x%x, ready=%d, available=%d\n", addr, driver->ready, fl3193_available[index]);
    return &driver->i2c_led;
}

void is31fl3193_uninit(i2c_led_t *driver)
{   
    if (!is31fl3193_available(driver->index)) return;

    // turn chip off
    uninit_driver(driver);

    // reset driver data
    is31fl3193_driver_t *is31 = (is31fl3193_driver_t*)(driver->data);
    
    memset(is31, 0, sizeof(is31fl3193_driver_t));
}

void is31fl3193_set_color(i2c_led_t *driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_led_t *led = &g_rgb_leds[index];
    is31fl3193_driver_t *is31 = (is31fl3193_driver_t*)(driver->data);
    if (is31->pwm_buffer[led->r + 1] != red) {
        is31->pwm_buffer[led->r + 1] = red;
        is31->pwm_dirty = true;
    }

    if (is31->pwm_buffer[led->g + 1] != green) {
        is31->pwm_buffer[led->g + 1] = green;
        is31->pwm_dirty = true;
    }

    if (is31->pwm_buffer[led->b + 1] != blue) {
        is31->pwm_buffer[led->b + 1] = blue;
        is31->pwm_dirty = true;
    }
}

void is31fl3193_set_color_all(i2c_led_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < driver->led_num; i++) {
        is31fl3193_set_color(driver, i, red, green, blue);
    }
}

bool is31fl3193_update_buffers(i2c_led_t *driver)
{
    if (!is31fl3193_available(driver->index)) return true;
   
    is31fl3193_driver_t *is31 = (is31fl3193_driver_t*)(driver->data);
    if (!is31->pwm_dirty) return true;

#ifdef RGB_FLUSH_ASYNC
    if (AMK_SUCCESS == ak_i2c_send_async(i2c_inst, driver->addr, is31->pwm_buffer, PWM_BUFFER_SIZE + 1)) {
#else
    if (AMK_SUCCESS == ak_i2c_send(i2c_inst, driver->addr, is31->pwm_buffer, PWM_BUFFER_SIZE + 1, TIMEOUT)) {
#endif
        is31->pwm_dirty = false;
        uint8_t data = 0;
        if (AMK_SUCCESS != ak_i2c_write_reg(i2c_inst, driver->addr, UPDATE_REG, &data, 1, TIMEOUT)) {
            fl3193_debug("IS31FL3193: failed to update UPDATE register for pwm\n");
        }
        //fl3193_debug("IS31FL3193: pwm UPDATE success\n");
        return true;
    }

    return false;
}


bool init_driver(is31fl3193_driver_t *driver)
{
    uint32_t status = AMK_SUCCESS;
    if (!i2c_inst) {
        i2c_inst = ak_i2c_init(IS31FL3193_I2C_ID);
    }

    memset(driver->pwm_buffer, 0, PWM_BUFFER_SIZE + 1);
    driver->pwm_buffer[0]       = PWM_REG;
    driver->pwm_dirty           = false;

    uint8_t data = 0;
    #if 0
    // Reset 3193 to default state
    status = ak_i2c_write_reg(i2c_inst, driver->i2c_led.addr, RESET_REG, &data, 1, TIMEOUT);
    if (status != AMK_SUCCESS) {
        fl3193_debug("IS31FL3193: failed to reset: addr:0x%x, status:%d\n", driver->i2c_led.addr, status);
        fl3193_available[driver->i2c_led.index] = false;
        return false;
    }

    wait_ms(10);
    #endif

    // Turn on chip and all channel
    data = 0x20;
    status = ak_i2c_write_reg(i2c_inst, driver->i2c_led.addr, SHUTDOWN_REG, &data, 1, TIMEOUT);
    if (status != AMK_SUCCESS) {
        fl3193_debug("IS31FL3193: failed to turn on chip: %d\n", status);
        fl3193_available[driver->i2c_led.index] = false;
        return false;
    }

    // Set current
    data = IS31FL3193_CURRENT;
    status = ak_i2c_write_reg(i2c_inst, driver->i2c_led.addr, CURRENT_REG, &data, 1, TIMEOUT);
    if (status != AMK_SUCCESS) {
        fl3193_debug("IS31FL3193: failed to set current=%d: error=%d\n", data, status);
        fl3193_available[driver->i2c_led.index] = false;
        return false;
    }

    // Clear pwm
    status = ak_i2c_send(i2c_inst, driver->i2c_led.addr, driver->pwm_buffer, PWM_BUFFER_SIZE+1, TIMEOUT);
    if (status != AMK_SUCCESS) {
        fl3193_debug("IS31FL3193: failed to clear pwm: %d\n", status);
        fl3193_available[driver->i2c_led.index] = false;
        return false;
    }

    // update PWM and control values
    data = 0;
    status = ak_i2c_write_reg(i2c_inst, driver->i2c_led.addr, UPDATE_REG, &data, 1, TIMEOUT);
    if (status != AMK_SUCCESS) {
        fl3193_debug("IS31FL3193: failed to update pwm: %d\n", status);
        fl3193_available[driver->i2c_led.index] = false;
        return false;
    }
    fl3193_available[driver->i2c_led.index] = true;
    return true;
}

void uninit_driver(i2c_led_t *driver)
{
    // shutdonw driver
    uint8_t data = 0;
    ak_i2c_write_reg(i2c_inst, driver->addr, RESET_REG, &data, 1, TIMEOUT);
}
