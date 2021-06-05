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

typedef struct {
    is31_t          is31;
    uint8_t         pwm_buffer[PWM_BUFFER_SIZE + 1];
    bool            pwm_dirty;
    uint8_t         control_buffer[CONTROL_BUFFER_SIZE + 1];
    bool            control_dirty;
} is31fl3731_driver_t;

static is31fl3731_driver_t is31_drivers[IS31_DRIVER_NUM] = {0};

static void write_register(uint8_t addr, uint8_t reg, uint8_t data);
static void init_driver(is31fl3731_driver_t *driver);
static void uninit_driver(is31_t *driver);

static void map_led(uint8_t index, uint8_t *red_reg, uint8_t* green_reg, uint8_t *blue_reg)
{
    rgb_led_t *led = &g_is31_leds[index];
    *red_reg    = led->r - PWM_REG;
    *green_reg  = led->g - PWM_REG;
    *blue_reg   = led->b - PWM_REG;
}

is31_t *is31fl3731_init(uint8_t addr, uint8_t led_num)
{
    is31_t *driver = NULL;
    for (int i = 0; i < IS31_DRIVER_NUM; i++) {
        if (is31_drivers[i].is31.addr == 0) {
            driver          = &(is31_drivers[i].is31);
            driver->addr    = addr;
            driver->led_num = led_num;
            driver->user    = &(is31_drivers[i]);
            init_driver(&is31_drivers[i]);
            break;
        }
    }
    return driver;
}

void is31fl3731_uninit(is31_t *driver)
{   
    // turn chip off
    uninit_driver(driver);

    // reset driver data
    is31fl3731_driver_t *is31 = (is31fl3731_driver_t*)(driver->user);
    memset(is31, 0, sizeof(is31fl3731_driver_t));

    for (int i = 0; i < IS31_DRIVER_NUM; i++) {
        if (is31_drivers[i].is31.addr != 0)
            return;
    }
    // all drivers uninited, release the i2c interface
    //if (i2c_ready()) i2c_uninit();
}

void is31fl3731_set_color(is31_t *driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t r, g, b;
    map_led(index, &r, &g, &b);
    is31fl3731_driver_t *is31 = (is31fl3731_driver_t*)(driver->user);
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
    is31fl3731_driver_t *is31 = (is31fl3731_driver_t*)(driver->user);
    if (is31->pwm_dirty) {
        i2c_send(driver->addr, is31->pwm_buffer, PWM_BUFFER_SIZE + 1, IS31_TIMEOUT);
        is31->pwm_dirty = false;
    }
}


void init_driver(is31fl3731_driver_t *driver)
{
    if (!i2c_ready()) i2c_init();

    memset(driver->pwm_buffer, 0, PWM_BUFFER_SIZE + 1);
    driver->pwm_buffer[0]       = PWM_REG;
    driver->pwm_dirty           = false;
    memset(driver->control_buffer, 0, CONTROL_BUFFER_SIZE + 1);
    driver->control_buffer[0]   = CONTROL_REG;
    driver->control_dirty       = false;

    // reset the 3731 to initial state
    write_register(driver->is31.addr, COMMAND_REG, BANK_FUNCTION_REG);
    write_register(driver->is31.addr, SHUTDOWN_REG, 0);

    // set mode
    write_register(driver->is31.addr, CONFIG_REG, PICTURE_MODE);
    write_register(driver->is31.addr, PICTURE_FRAME_REG, 0);
    write_register(driver->is31.addr, AUDIO_SYNC_REG, 0);

    write_register(driver->is31.addr, COMMAND_REG, 0);
    // turn off leds, blinks, pwms
    for (int i = 0; i < 0xB4; i++) {
        write_register(driver->is31.addr, i, 0);
    }

    write_register(driver->is31.addr, COMMAND_REG, BANK_FUNCTION_REG);
    write_register(driver->is31.addr, SHUTDOWN_REG, 1);
    write_register(driver->is31.addr, COMMAND_REG, 0);

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
    i2c_send(driver->is31.addr, driver->control_buffer, CONTROL_BUFFER_SIZE+1, IS31_TIMEOUT);
}

void uninit_driver(is31_t *driver)
{
    // shutdonw driver
    write_register(driver->addr, COMMAND_REG, BANK_FUNCTION_REG);
    write_register(driver->addr, SHUTDOWN_REG, 0);
}

void write_register(uint8_t addr, uint8_t reg, uint8_t data)
{
    i2c_write_reg(addr, reg, &data, 1, IS31_TIMEOUT);
}
