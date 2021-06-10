/**
 * is31fl3731.c
 *  driver implementation for is31fl3731
 */

#include <stdbool.h>
#include <string.h>
#include "is31fl3731.h"
#include "i2c.h"

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

#ifndef IS31FL3731_I2C_ID
#define IS31FL3731_I2C_ID     I2C_INSTANCE_1
#endif
static i2c_handle_t i2c_inst;

typedef struct {
    is31_t          is31;
    uint8_t         pwm_buffer[PWM_BUFFER_SIZE + 1];
    bool            pwm_dirty;
    uint8_t         control_buffer[CONTROL_BUFFER_SIZE + 1];
    bool            control_dirty;
    bool            ready;
} is31fl3731_driver_t;

static is31fl3731_driver_t is31_drivers[IS31_DRIVER_NUM] = {0};

static void init_driver(is31fl3731_driver_t *driver);
static void uninit_driver(is31_t *driver);

static void map_led(uint8_t index, uint8_t *red_reg, uint8_t* green_reg, uint8_t *blue_reg)
{
    rgb_led_t *led = &g_rgb_leds[index];
    *red_reg    = led->r - PWM_REG;
    *green_reg  = led->g - PWM_REG;
    *blue_reg   = led->b - PWM_REG;
}

is31_t *is31fl3731_init(uint8_t addr, uint8_t index, uint8_t led_num)
{
    is31fl3731_driver_t *driver = &is31_drivers[index];
    driver->is31.addr = addr;
    driver->is31.index = index;
    driver->is31.led_num = led_num;
    driver->is31.data = driver;

    init_driver(driver);

    driver->ready = true;
    return &driver->is31;
}

void is31fl3731_uninit(is31_t *driver)
{   
    // turn chip off
    uninit_driver(driver);

    // reset driver data
    is31fl3731_driver_t *is31 = (is31fl3731_driver_t*)(driver->data);
    memset(is31, 0, sizeof(is31fl3731_driver_t));
}

void is31fl3731_set_color(is31_t *driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t r, g, b;
    map_led(index, &r, &g, &b);
    is31fl3731_driver_t *is31 = (is31fl3731_driver_t*)(driver->data);
    is31->pwm_buffer[r + 1] = red;
    is31->pwm_buffer[g + 1] = green;
    is31->pwm_buffer[b + 1] = blue;
    is31->pwm_dirty = true;
}

void is31fl3731_set_color_all(is31_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < driver->led_num; i++) {
        is31fl3731_set_color(driver, i, red, green, blue);
    }
}

void is31fl3731_update_buffers(is31_t *driver)
{
    is31fl3731_driver_t *is31 = (is31fl3731_driver_t*)(driver->data);
    if (is31->pwm_dirty) {
        i2c_send(i2c_inst, driver->addr, is31->pwm_buffer, PWM_BUFFER_SIZE + 1, IS31_TIMEOUT);
        is31->pwm_dirty = false;
    }
}


void init_driver(is31fl3731_driver_t *driver)
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
    i2c_write_reg(i2c_inst, driver->is31.addr, COMMAND_REG, &data, 1, TIMEOUT);
    data = 0;
    i2c_write_reg(i2c_inst, driver->is31.addr, SHUTDOWN_REG, &data, 1, TIMEOUT);

    // set mode
    data = PICTURE_MODE;
    i2c_write_reg(i2c_inst, driver->is31.addr, CONFIG_REG, &data, 1, TIMEOUT);
    data = 0;
    i2c_write_reg(i2c_inst, driver->is31.addr, PICTURE_FRAME_REG, &data, 1, TIMEOUT);
    i2c_write_reg(i2c_inst, driver->is31.addr, AUDIO_SYNC_REG, &data, 1, TIMEOUT);

    i2c_write_reg(i2c_inst, driver->is31.addr, COMMAND_REG, &data, 1, TIMEOUT);
    // turn off leds, blinks, pwms
    for (int i = 0; i < 0xB4; i++) {
        i2c_write_reg(i2c_inst, driver->is31.addr, i, &data, 1, TIMEOUT);
    }

    data = BANK_FUNCTION_REG;
    i2c_write_reg(i2c_inst, driver->is31.addr, COMMAND_REG, &data, 1, TIMEOUT);
    data = 1;
    i2c_write_reg(i2c_inst, driver->is31.addr, SHUTDOWN_REG, &data, 1, TIMEOUT);
    data = 0;
    i2c_write_reg(i2c_inst, driver->is31.addr, COMMAND_REG, &data, 1, TIMEOUT);

    // turn on used leds
    for (int i = 0; i < driver->is31.led_num; i++) {
        uint8_t r, g, b;
        map_led(i, &r, &g, &b);

        uint8_t reg_r = r / 8;
        uint8_t reg_g = g / 8;
        uint8_t reg_b = b / 8;
        uint8_t bit_r = r % 8;
        uint8_t bit_g = g % 8;
        uint8_t bit_b = b % 8;
        driver->control_buffer[reg_r+1] |= (1 << bit_r);
        driver->control_buffer[reg_g+1] |= (1 << bit_g);
        driver->control_buffer[reg_b+1] |= (1 << bit_b);
    }
    i2c_send(i2c_inst, driver->is31.addr, driver->control_buffer, CONTROL_BUFFER_SIZE+1, TIMEOUT);
}

void uninit_driver(is31_t *driver)
{
    // shutdonw driver
    uint8_t data = BANK_FUNCTION_REG;
    i2c_write_reg(i2c_inst, driver->addr, COMMAND_REG, &data, 1, TIMEOUT);
    data = 0;
    i2c_write_reg(i2c_inst, driver->addr, SHUTDOWN_REG, &data, 1, TIMEOUT);
}

