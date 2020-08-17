/**
 * is31fl3731.c
 *  driver implementation for is31fl3731
 */

#include "is31fl3731.h"
#include "i2c.h"

#ifndef IS31_DRIVER_NUM
#define IS31_DRIVER_NUM     2
#endif

#define DEFAULT_TIMEOUT     100

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
    is31fl3731_t is31;
    uint8_t pwm_buffer[PWM_BUFFER_SIZE+1];
    uint8_t control_buffer[CONTROL_BUFFER_SIZE+1];
} is31fl3731_driver_t;

static is31fl3731_driver_t is31_drivers[IS31_DRIVER_NUM] = {{0}};

static void write_register(uint8_t addr, uint8_t reg, uint8_t data);
static void init_drive(is31fl3731_driver_t *driver);
static is31fl3731_driver_t *find_driver(is31fl3731_t *driver);

is31fl3731_t* is31fl3731_init(uint8_t addr, uint8_t led_num, map_led_t map_led)
{
    is31fl3731_t* driver = NULL;
    for (int i = 0; i < IS31_DRIVER_NUM; i++) {
        if (is31_drivers[i].addr != 0) {
            driver = &(is31_drivers[i].driver);
            driver->addr = addr;
            driver->led_num = led_num;
            driver->map_led = map_led;
            init_driver(&is31_drivers[i]);
            break;
        }
    }
    return driver;
}

void is31fl3731_set_color(is31fl3731_t* driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t r, g, b;
    driver->map_led(index, &r, &g, &b);
    is31fl3731_driver_t* is31 = find_driver(driver);
    is31->pwm_buffer[r-PWM_REG+ 1] = r;
    is31->pwm_buffer[g-PWM_REG+ 1] = g;
    is31->pwm_buffer[b-PWM_REG+ 1] = b;
}

void is31fl3731_set_color_all(is31fl3731_t* driver, uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < driver->led_num; i++) {
        is31fl3731_set_color(driver, i, red, green, blue);
    }
}

void is31fl3731_update_buffers(is31fl3731_t* driver)
{
    is31fl3731_driver_t *is31 = find_driver(driver);
    i2c_send(driver->addr, is31->pwm_buffer, PWM_BUFFER_SIZE + 1, DEFAULT_TIMEOUT);
}

void is31fl3731_uninit(is31fl3731_t *led_driver)
{
}

void init_drive(is31fl3731_driver_t *driver)
{
    memset(driver->pwm_buffer, 0, PWM_BUFFER_SIZE);
    driver->pwm_buffer[0] = PWM_REG;
    memset(driver->control_buffer, 0, CONTROL_BUFFER_SIZE);
    driver->control_buffer[0] = CONTROL_REG;

    i2c_init();
    // reset the 3731 to initial state
    write_register(driver->is31.addr, COMMAND_REG, BANK_FUNCTION_REG);
    write_register(driver->is31.addr, SHUTDOWN_REG, 0);

    // set mode
    write_register(driver->is31.addr, CONFIG_REG, PICTURE_MODE);
    write_register(driver->is31.addr, PICTURE_FRAME_REG, 0);
    write_register(driver->is31.addr, AUDIO_SYNC_REG, 0);

    write_register(driver->is31.addr, COMMAND_REG, 0);
    for (int i = 0; i < 0xB4; i++) {
        write_register(driver->is31.addr, i, 0);
    }

    write_register(driver->is31.addr, COMMAND_REG, BANK_FUNCTION_REG);
    write_register(driver->is31.addr, SHUTDOWN_REG, 1);
    write_register(driver->is31.addr, COMMAND_REG, 0);

    for (int i = 0; i < driver->is31.led_num; i++) {
        uint8_t r, g, b;
        driver->is31.map_led(i, &r, &g, &b);

        uint8_t reg_r = (r - 0x24) / 8;
        uint8_t reg_g = (g - 0x24) / 8;
        uint8_t reg_b = (b - 0x24) / 8;
        uint8_t bit_r = (r - 0x24) % 8;
        uint8_t bit_g = (g - 0x24) % 8;
        uint8_t bit_b = (b - 0x24) % 8;
        driver->control_buffer[reg_r+1] |= (1 << bit_r);
        driver->control_buffer[reg_g+1] |= (1 << bit_g);
        driver->control_buffer[reg_b+1] |= (1 << bit_b);
    }
    i2c_send(driver->is31.addr, driver->control_buffer, CONTROL_BUFFER_SIZE+1, DEFAULT_TIMEOUT);
}

void write_register(uint8_t addr, uint8_t reg, uint8_t data)
{
    i2c_write_reg(addr, reg, &data, 1, DEFAULT_TIMEOUT);
}

is31fl3731_driver_t *find_driver(is31fl3731_t *driver)
{
    for (int i = 0; i < IS31_DRIVER_NUM; i++) {
        if (&(is31_drivers[i].is31) == driver) {
            return &is31_drivers[i];
        }
    }

    return NULL;
}