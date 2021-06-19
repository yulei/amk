/**
 * @file is31fl3236.c
 * @brief driver implementation for is31fl3236
 */

#include <string.h>
#include "is31fl3236.h"
#include "rgb_common.h"
#include "i2c.h"
#include "wait.h"
#include "amk_printf.h"

#ifndef FL3236_DEBUG
#define FL3236_DEBUG 0
#endif

#if FL3236_DEBUG
#define fl3236_debug  amk_printf
#else
#define fl3236_debug(...)
#endif

#define PWM_BUFFER_SIZE     36
#define CONTROL_BUFFER_SIZE 36

#define SHUTDOWN_REG        0x00
#define PWM_REG             0x01
#define UPDATE_REG          0x25
#define CONTROL_REG         0x26
#define GLOBAL_CONTROL_REG  0x4A
#define OUT_FREQUENCY_REG   0x4B
#define RESET_REG           0x4F

#ifndef IS31FL3236_NUM
    #define IS31FL3236_NUM  1
#endif

#define TIMEOUT             100
#ifndef IS31FL3236_I2C_ID
#define IS31FL3236_I2C_ID     I2C_INSTANCE_1
#endif

static i2c_handle_t i2c_inst;

typedef struct {
    i2c_led_t       i2c_led;
    uint8_t         pwm_buffer[PWM_BUFFER_SIZE + 1];
    bool            pwm_dirty;
    uint8_t         control_buffer[CONTROL_BUFFER_SIZE + 1];
    bool            control_dirty;
    bool            ready;
} is31fl3236_driver_t;

static is31fl3236_driver_t is31_drivers[IS31FL3236_NUM] = {0};

static void init_driver(is31fl3236_driver_t *driver);
static void uninit_driver(i2c_led_t *driver);

i2c_led_t *is31fl3236_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num)
{
    is31fl3236_driver_t *driver = &is31_drivers[index];
    if (driver->ready) return &driver->i2c_led;

    driver->i2c_led.addr        = addr;
    driver->i2c_led.index       = index;
    driver->i2c_led.led_start   = led_start;
    driver->i2c_led.led_num     = led_num;
    driver->i2c_led.data        = driver;

    init_driver(driver);

    driver->ready = true;
    fl3236_debug("IS31FL3236: initialized\n");
    return &driver->i2c_led;
}

void is31fl3236_uninit(i2c_led_t *driver)
{   
    // turn chip off
    uninit_driver(driver);

    // reset driver data
    is31fl3236_driver_t *is31 = (is31fl3236_driver_t*)(driver->data);
    
    memset(is31, 0, sizeof(is31fl3236_driver_t));
}

void is31fl3236_set_color(i2c_led_t *driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_led_t *led = &g_rgb_leds[index];
    is31fl3236_driver_t *is31 = (is31fl3236_driver_t*)(driver->data);
    is31->pwm_buffer[led->r + 1] = red;
    is31->pwm_buffer[led->g + 1] = green;
    is31->pwm_buffer[led->b + 1] = blue;
    is31->pwm_dirty = true;
}

void is31fl3236_set_color_all(i2c_led_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < driver->led_num; i++) {
        is31fl3236_set_color(driver, i, red, green, blue);
    }
}

void is31fl3236_update_buffers(i2c_led_t *driver)
{
    uint32_t status = AMK_SUCCESS;
    is31fl3236_driver_t *is31 = (is31fl3236_driver_t*)(driver->data);
    bool need_update = is31->pwm_dirty || is31->control_dirty;
    if (is31->pwm_dirty) {
        status = i2c_send(i2c_inst, driver->addr, is31->pwm_buffer, PWM_BUFFER_SIZE + 1, TIMEOUT);
        if (status != AMK_SUCCESS) {
            fl3236_debug("IS31FL3236: failed to update pwm buffer: addr=%d, status=%d\n", driver->addr, status);
        }
        is31->pwm_dirty = false;
    }

    if (is31->control_dirty) {
        status = i2c_send(i2c_inst, driver->addr, is31->control_buffer, CONTROL_BUFFER_SIZE + 1, TIMEOUT);
        if (status != AMK_SUCCESS) {
            fl3236_debug("IS31FL3236: failed to update control buffer: %d\n", status);
        }
        is31->pwm_dirty = false;
    }

    if (need_update) {
        uint8_t data = 0;
        status = i2c_write_reg(i2c_inst, driver->addr, UPDATE_REG, &data, 1, TIMEOUT);
        if (status != AMK_SUCCESS) {
            fl3236_debug("IS31FL3236: failed to update UPDATE register: %d\n", status);
        }
    }
}


void init_driver(is31fl3236_driver_t *driver)
{
    uint32_t status = AMK_SUCCESS;
    if (!i2c_inst) {
        i2c_inst = i2c_init(IS31FL3236_I2C_ID);
    }

    memset(driver->pwm_buffer, 0, PWM_BUFFER_SIZE + 1);
    driver->pwm_buffer[0]       = PWM_REG;
    driver->pwm_dirty           = false;
    memset(driver->control_buffer, 1, CONTROL_BUFFER_SIZE + 1);
    driver->control_buffer[0]   = CONTROL_REG;
    driver->control_dirty       = false;

    // Reset 3236 to default state
    uint8_t data = 0;
    status = i2c_write_reg(i2c_inst, driver->i2c_led.addr, RESET_REG, &data, 1, TIMEOUT);
    if (status != AMK_SUCCESS) {
        fl3236_debug("IS31FL3236: failed to reset: %d\n", status);
    }

    wait_ms(10);

    // Turn on chip
    data = 1;
    status = i2c_write_reg(i2c_inst, driver->i2c_led.addr, SHUTDOWN_REG, &data, 1, TIMEOUT);
    if (status != AMK_SUCCESS) {
        fl3236_debug("IS31FL3236: failed to turn on chip: %d\n", status);
    }

    // Turn on all leds
    status = i2c_send(i2c_inst, driver->i2c_led.addr, driver->control_buffer, CONTROL_BUFFER_SIZE+1, TIMEOUT);
    if (status != AMK_SUCCESS) {
        fl3236_debug("IS31FL3236: failed to turn on leds: %d\n", status);
    }

    // Clear pwm
    status = i2c_send(i2c_inst, driver->i2c_led.addr, driver->pwm_buffer, PWM_BUFFER_SIZE+1, TIMEOUT);
    if (status != AMK_SUCCESS) {
        fl3236_debug("IS31FL3236: failed to clear pwm: %d\n", status);
    }

    // update PWM and control values
    data = 0;
    status = i2c_write_reg(i2c_inst, driver->i2c_led.addr, UPDATE_REG, &data, 1, TIMEOUT);
    if (status != AMK_SUCCESS) {
        fl3236_debug("IS31FL3236: failed to update pwm&control: %d\n", status);
    }
}

void uninit_driver(i2c_led_t *driver)
{
    // shutdonw driver
    uint8_t data = 0;
    i2c_write_reg(i2c_inst, driver->addr, RESET_REG, &data, 1, TIMEOUT);
}
