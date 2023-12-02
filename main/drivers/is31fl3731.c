/**
 * is31fl3731.c
 *  driver implementation for is31fl3731
 */

#include <stdbool.h>
#include <string.h>
#include "is31fl3731.h"
#include "rgb_common.h"
#include "i2c.h"
#include "wait.h"
#include "amk_printf.h"

#define PWM_BUFFER_SIZE     144
#define CONTROL_BUFFER_SIZE 18

#define COMMAND_REG         0xFD
#define CONFIG_REG          0
#define PICTURE_MODE        0
#define PICTURE_FRAME_REG   0x01
#define AUDIO_SYNC_REG      0x06
#define SHUTDOWN_REG        0x0A
#define BANK_FUNCTION_REG   0x0B

#define CONTROL_REG         0
#define PWM_REG             0x24
#define TIMEOUT             100

#ifndef IS31FL3731_NUM
    #define IS31FL3731_NUM  1
#endif

#ifndef IS31FL3731_I2C_ID
#define IS31FL3731_I2C_ID     I2C_INSTANCE_1
#endif
static i2c_handle_t i2c_inst;

typedef struct {
    i2c_led_t       i2c_led;
    uint8_t         pwm_buffer[PWM_BUFFER_SIZE + 1];
    bool            pwm_dirty;
    uint8_t         control_buffer[CONTROL_BUFFER_SIZE + 1];
    bool            control_dirty;
    bool            ready;
} is31fl3731_driver_t;

static is31fl3731_driver_t is31_drivers[IS31FL3731_NUM];
static bool fl3731_available[IS31FL3731_NUM] = {false};

static bool init_driver(is31fl3731_driver_t *driver);
static void uninit_driver(i2c_led_t *driver);

bool is31fl3731_available(uint8_t index)
{
    return fl3731_available[index];
}

i2c_led_t *is31fl3731_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num)
{
    is31fl3731_driver_t *driver = &is31_drivers[index];
    if (driver->ready) return &driver->i2c_led;

    driver->i2c_led.addr        = addr;
    driver->i2c_led.index       = index;
    driver->i2c_led.led_start   = led_start;
    driver->i2c_led.led_num     = led_num;
    driver->i2c_led.data        = driver;

    driver->ready = init_driver(driver);

    return &driver->i2c_led;
}

void is31fl3731_uninit(i2c_led_t *driver)
{   
    if (!is31fl3731_available(driver->index)) return;

    // turn chip off
    uninit_driver(driver);

    // reset driver data
    is31fl3731_driver_t *is31 = (is31fl3731_driver_t*)(driver->data);
    memset(is31, 0, sizeof(is31fl3731_driver_t));
}

void is31fl3731_set_color(i2c_led_t *driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    if (!is31fl3731_available(driver->index)) return;

    rgb_led_t *led = &g_rgb_leds[index];
    is31fl3731_driver_t *is31 = (is31fl3731_driver_t*)(driver->data);
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
    //is31->pwm_dirty = true;
}

void is31fl3731_set_color_all(i2c_led_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    if (!is31fl3731_available(driver->index)) return;

    for (int i = 0; i < driver->led_num; i++) {
        is31fl3731_set_color(driver, i, red, green, blue);
    }
}

void is31fl3731_update_buffers(i2c_led_t *driver)
{
    if (!is31fl3731_available(driver->index)) return;

    is31fl3731_driver_t *is31 = (is31fl3731_driver_t*)(driver->data);
    if (is31->pwm_dirty) {
        if (AMK_SUCCESS == i2c_send_async(i2c_inst, driver->addr, is31->pwm_buffer, PWM_BUFFER_SIZE + 1)) {
            is31->pwm_dirty = false;
            amk_printf("IS31FL3731: i2c_send success\n");
        }
    }
}

bool init_driver(is31fl3731_driver_t *driver)
{
    if (!i2c_inst) {
        i2c_inst = i2c_init(IS31FL3731_I2C_ID);
    }

    memset(driver->pwm_buffer, 0, PWM_BUFFER_SIZE + 1);
    driver->pwm_buffer[0]       = PWM_REG;
    driver->pwm_dirty           = false;
    memset(driver->control_buffer, 0, CONTROL_BUFFER_SIZE + 1);
    driver->control_buffer[0]   = CONTROL_REG;
    driver->control_dirty       = false;

    // reset the 3731 to initial state
    uint8_t data = BANK_FUNCTION_REG;
    if ( AMK_SUCCESS != i2c_write_reg(i2c_inst, driver->i2c_led.addr, COMMAND_REG, &data, 1, TIMEOUT)) {
        amk_printf("IS31FL3731: failed to select FUNCTION page\n");
        //return false;
    }

    if ( AMK_SUCCESS != i2c_write_reg(i2c_inst, driver->i2c_led.addr, COMMAND_REG, &data, 1, TIMEOUT)) {
        amk_printf("IS31FL3731: failed to select FUNCTION page again\n");
        return false;
    }

    data = 0;
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, SHUTDOWN_REG, &data, 1, TIMEOUT);

    wait_ms(10);

    // set mode
    data = PICTURE_MODE;
    if (AMK_SUCCESS != i2c_write_reg(i2c_inst, driver->i2c_led.addr, CONFIG_REG, &data, 1, TIMEOUT)) {
        amk_printf("IS31FL3731: failed to write PICTURE mode\n");
    }
    data = 0;
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, PICTURE_FRAME_REG, &data, 1, TIMEOUT);
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, AUDIO_SYNC_REG, &data, 1, TIMEOUT);

    // select bank 0
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, COMMAND_REG, &data, 1, TIMEOUT);
    // turn off leds
    i2c_send(i2c_inst, driver->i2c_led.addr, driver->pwm_buffer, PWM_BUFFER_SIZE + 1, TIMEOUT);

    data = BANK_FUNCTION_REG;
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, COMMAND_REG, &data, 1, TIMEOUT);
    // enable chip
    data = 1;
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, SHUTDOWN_REG, &data, 1, TIMEOUT);
    // select bank 0
    data = 0;
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, COMMAND_REG, &data, 1, TIMEOUT);

    // turn on used leds
    for (int i = 0; i < driver->i2c_led.led_num; i++) {
        rgb_led_t *led = &g_rgb_leds[i+driver->i2c_led.led_start];

        uint8_t reg_r = led->r / 8;
        uint8_t reg_g = led->g / 8;
        uint8_t reg_b = led->b / 8;
        uint8_t bit_r = led->r % 8;
        uint8_t bit_g = led->g % 8;
        uint8_t bit_b = led->b % 8;
        driver->control_buffer[reg_r+1] |= (1 << bit_r);
        driver->control_buffer[reg_g+1] |= (1 << bit_g);
        driver->control_buffer[reg_b+1] |= (1 << bit_b);
    }
    i2c_send(i2c_inst, driver->i2c_led.addr, driver->control_buffer, CONTROL_BUFFER_SIZE+1, TIMEOUT);

    fl3731_available[driver->i2c_led.index] = true;
    return true;
}

void uninit_driver(i2c_led_t *driver)
{
    if (!is31fl3731_available(driver->index)) return;

    // shutdonw driver
    uint8_t data = BANK_FUNCTION_REG;
    i2c_write_reg(i2c_inst, driver->addr, COMMAND_REG, &data, 1, TIMEOUT);
    data = 0;
    i2c_write_reg(i2c_inst, driver->addr, SHUTDOWN_REG, &data, 1, TIMEOUT);
}

