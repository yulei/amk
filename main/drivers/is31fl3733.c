/**
 * @file is31fl3733.c
 *
 */

#include <stdbool.h>
#include <string.h>
#include "is31fl3733.h"
#include "rgb_common.h"
#include "wait.h"
#include "i2c.h"

#define COMMAND_REG                 0xFD
#define COMMAND_WRITE_LOCK_REG      0xFE
#define INTERRUPT_MASK_REG          0xF0
#define INTERRUPT_STATUS_REG        0xF1

#define PAGE_CONTROL                0x00
#define PAGE_PWM                    0x01
#define PAGE_AUTO_BREATH            0x02
#define PAGE_FUNCTION               0x03

// function page
#define FUNCTION_CONFIGURATION_REG  0x00
#define FUNCTION_GLOBAL_CURRENT_REG 0x01
//0x02~0x0E for auto breath

#define FUNCTION_SW_PULLUP_REG      0x0F
#define FUNCTION_CS_PULLUP_REG      0x10
#define FUNCTION_RESET_REG          0x11

#define PWM_BUFFER_SIZE             0xC0
#define CONTROL_BUFFER_SIZE         0x18
#define COMMAND_UNLOCK              0xC5

#define TIMEOUT                     100

#ifndef IS31FL3733_NUM
    #define IS31FL3733_NUM  1
#endif

#ifndef IS31FL3733_I2C_ID
#define IS31FL3733_I2C_ID     I2C_INSTANCE_1
#endif
static i2c_handle_t i2c_inst;

typedef struct {
    i2c_led_t       i2c_led;
    uint8_t         pwm_buffer[PWM_BUFFER_SIZE+1];
    bool            pwm_dirty;
    uint8_t         control_buffer[CONTROL_BUFFER_SIZE];
    bool            control_dirty;
    bool            ready;
} is31fl3733_driver_t;

static is31fl3733_driver_t is31_drivers[IS31FL3733_NUM];

static void init_driver(is31fl3733_driver_t *driver);
static void uninit_driver(i2c_led_t *driver);

i2c_led_t *is31fl3733_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num)
{
    is31fl3733_driver_t *driver = &is31_drivers[index];
    if (driver->ready) return &driver->i2c_led;

    driver->i2c_led.addr        = addr;
    driver->i2c_led.index       = index;
    driver->i2c_led.led_start   = led_start;
    driver->i2c_led.led_num     = led_num;
    driver->i2c_led.data        = driver;

    init_driver(driver);

    driver->ready = true;
    return &driver->i2c_led;
}

void is31fl3733_uninit(i2c_led_t *driver)
{
    // turn chip off
    uninit_driver(driver);

    // reset driver data
    is31fl3733_driver_t *is31 = (is31fl3733_driver_t*)(driver->data);
    memset(is31, 0, sizeof(is31fl3733_driver_t));
}

void is31fl3733_set_color(i2c_led_t *driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_led_t *led = &g_rgb_leds[index];
    is31fl3733_driver_t *is31 = (is31fl3733_driver_t*)(driver->data);
    is31->pwm_buffer[led->r+1] = red;
    is31->pwm_buffer[led->g+1] = green;
    is31->pwm_buffer[led->b+1] = blue;
    is31->pwm_dirty = true;
}

void is31fl3733_set_color_all(i2c_led_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < driver->led_num; i++) {
        is31fl3733_set_color(driver, i, red, green, blue);
    }
}

void is31fl3733_update_buffers(i2c_led_t *driver)
{
    is31fl3733_driver_t *is31 = (is31fl3733_driver_t*)(driver->data);
    if (is31->pwm_dirty) {
        uint8_t data = COMMAND_UNLOCK;
        i2c_write_reg(i2c_inst, driver->addr, COMMAND_WRITE_LOCK_REG, &data, 1, TIMEOUT);

        // select pwm page
        data = PAGE_PWM;
        i2c_write_reg(i2c_inst, driver->addr, COMMAND_REG, &data, 1, TIMEOUT);

        i2c_send(i2c_inst, driver->addr, is31->pwm_buffer, PWM_BUFFER_SIZE+1, TIMEOUT);
        is31->pwm_dirty = false;
    }
}

static void init_driver(is31fl3733_driver_t *driver)
{
    if (!i2c_inst) {
        i2c_inst = i2c_init(IS31FL3733_I2C_ID);
    }

    memset(driver->pwm_buffer, 0, PWM_BUFFER_SIZE+1);
    driver->pwm_dirty           = false;
    memset(driver->control_buffer, 0, CONTROL_BUFFER_SIZE);
    driver->control_dirty       = false;

    // unlock 
    uint8_t data = COMMAND_UNLOCK;
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, COMMAND_WRITE_LOCK_REG, &data, 1, TIMEOUT);

    // select control page  
    data = PAGE_CONTROL;
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, COMMAND_REG, &data, 1, TIMEOUT);

    // enable all leds
    data = 0xFF;
    for (int i = 0; i < CONTROL_BUFFER_SIZE; i++) {
        i2c_write_reg(i2c_inst, driver->i2c_led.addr, i, &data, 1, TIMEOUT);
    }

    // unlock
    data = COMMAND_UNLOCK;
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, COMMAND_WRITE_LOCK_REG, &data, 1, TIMEOUT);

    // select pwm page
    data = PAGE_PWM;
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, COMMAND_REG, &data, 1, TIMEOUT);

    // set all pwm
    i2c_send(i2c_inst, driver->i2c_led.addr, driver->pwm_buffer, PWM_BUFFER_SIZE+1, TIMEOUT);

    // unlock
    data = COMMAND_UNLOCK;
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, COMMAND_WRITE_LOCK_REG, &data, 1, TIMEOUT);

    // select function page
    data = PAGE_FUNCTION;
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, COMMAND_REG, &data, 1, TIMEOUT);

    // set gloabl current
    data = 0xFF;
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, FUNCTION_GLOBAL_CURRENT_REG, &data, 1, TIMEOUT);

    // enable the chip
    data = 1;
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, FUNCTION_CONFIGURATION_REG, &data, 1, TIMEOUT);

    wait_ms(10);
}

static void uninit_driver(i2c_led_t *driver)
{
    // unlock
    uint8_t data = COMMAND_UNLOCK;
    i2c_write_reg(i2c_inst, driver->addr, COMMAND_WRITE_LOCK_REG, &data, 1, TIMEOUT);

    // select function page
    data = PAGE_FUNCTION;
    i2c_write_reg(i2c_inst, driver->addr, COMMAND_REG, &data, 1, TIMEOUT);

    // disable the chip
    data = 0;
    i2c_write_reg(i2c_inst, driver->addr, FUNCTION_CONFIGURATION_REG, &data, 1, TIMEOUT);
}
