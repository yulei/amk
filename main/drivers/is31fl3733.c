/**
 * is31fl3733.c
 *  driver interace for is31fl3733
 */

#include <stdbool.h>
#include <string.h>
#include "is31fl3733.h"
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

#define PWM_BUFFER_SIZE             0xBF
#define CONTROL_BUFFER_SIZE         0x17
#define COMMAND_UNLOCK              0xC5

#define TIMEOUT                     100

#ifndef IS31FL3733_I2C_ID
#define IS31FL3733_I2C_ID     I2C_INSTANCE_1
#endif
//static i2c_handle_t i2c_inst;

typedef struct {
    is31_t          is31;
    uint8_t         pwm_buffer[PWM_BUFFER_SIZE+1];
    bool            pwm_dirty;
    uint8_t         control_buffer[CONTROL_BUFFER_SIZE+1];
    bool            control_dirty;
    bool            ready;
} is31fl3733_driver_t;

static is31fl3733_driver_t is31_drivers[IS31_DRIVER_NUM] = {0};

static void init_driver(is31fl3733_driver_t *driver);
static void uninit_driver(is31_t *driver);

static void map_led(uint8_t index, uint8_t *red_reg, uint8_t* green_reg, uint8_t *blue_reg)
{
    rgb_led_t *led = &g_rgb_leds[index];
    *red_reg    = led->r;
    *green_reg  = led->g;
    *blue_reg   = led->b;
}

is31_t *is31fl3733_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num)
{
    is31fl3733_driver_t *driver = &is31_drivers[index];
    driver->is31.addr = addr;
    driver->is31.index = index;
    driver->is31.led_start = led_start;
    driver->is31.led_num = led_num;
    driver->is31.data = driver;

    init_driver(driver);

    driver->ready = true;
    return &driver->is31;
}

void is31fl3733_uninit(is31_t *driver)
{
    // turn chip off
    uninit_driver(driver);

    // reset driver data
    is31fl3733_driver_t *is31 = (is31fl3733_driver_t*)(driver->data);
    memset(is31, 0, sizeof(is31fl3733_driver_t));
}

void is31fl3733_set_color(is31_t* driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t r, g, b;
    map_led(index, &r, &g, &b);
    is31fl3733_driver_t *is31 = (is31fl3733_driver_t*)(driver->data);
    is31->pwm_buffer[r + 1] = red;
    is31->pwm_buffer[g + 1] = green;
    is31->pwm_buffer[b + 1] = blue;
    is31->pwm_dirty = true;
}

void is31fl3733_set_color_all(is31_t* driver, uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < driver->led_num; i++) {
        is31fl3733_set_color(driver, i, red, green, blue);
    }
}

void is31fl3733_update_buffers(is31_t* driver)
{
}


static void init_driver(is31fl3733_driver_t *driver)
{
}

static void uninit_driver(is31_t *driver)
{
}