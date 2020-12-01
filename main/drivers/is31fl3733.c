/**
 * is31fl3733.c
 *  driver interace for is31fl3733
 */

#include <stdbool.h>
#include "is31fl3733.h"
#include "wait.h"

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

typedef struct {
    is31_t          is31;
    uint8_t         pwm_buffer[PWM_BUFFER_SIZE+1];
    bool            pwm_dirty;
    uint8_t         control_buffer[CONTROL_BUFFER_SIZE+1];
    bool            control_dirty;
} is31fl3733_driver_t;

static is31fl3733_driver_t is31_drivers[IS31_DRIVER_NUM] = {0};

static void write_register(uint8_t addr, uint8_t reg, uint8_t data);
static void read_register(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t size);
static void init_driver(is31fl3733_driver_t *driver);
static void uninit_driver(is31_t *driver);

static void map_led(uint8_t index, uint8_t *red_reg, uint8_t* green_reg, uint8_t *blue_reg)
{
    is31_led_t *led = &g_rgb_matrix.leds[index];
    *red_reg    = led->red + 1;
    *green_reg  = led->green + 1;
    *blue_reg   = led->blue + 1;
}

is31_t *is31fl3733_init(uint8_t addr, uint8_t led_num)
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

void is31fl3733_set_color(is31_t* driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t r, g, b;
    map_led(index, &r, &g, &b);
    is31fl3733_driver_t *is31 = (is31fl3733_driver_t*)(driver->user);
    is31->pwm_buffer[r] = red;
    is31->pwm_buffer[g] = green;
    is31->pwm_buffer[b] = blue;
    is31->pwm_dirty = true;
}

void is31fl3733_set_color_all(is31_t* driver, uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < driver->led_num; i++) {
        is31fl3731_set_color(driver, i, red, green, blue);
    }
}

void is31fl3733_update_buffers(is31_t* driver)
{
    is31fl3733_driver_t *is31 = (is31fl3733_driver_t*)(driver->user);
    if (is31->pwm_dirty) {
        // command register unlock
        write_register(driver->addr, COMMAND_WRITE_LOCK_REG, COMMAND_UNLOCK);
        // select pwm page
        write_register(driver->addr, COMMAND_REG, PAGE_PWM);
        // write pwm data
        i2c_send(driver->addr, is31->pwm_buffer, PWM_BUFFER_SIZE + 1, IS31_TIMEOUT);
        is31->pwm_dirty = false;
    }
}

void is31fl3733_uninit(is31_t *driver)
{
    // turn chip off
    uninit_driver(driver);

    // reset driver data
    is31fl3733_driver_t *is31 = (is31fl3733_driver_t*)(driver->user);
    memset(is31, 0, sizeof(is31fl3733_driver_t));

    for (int i = 0; i < IS31_DRIVER_NUM; i++) {
        if (is31_drivers[i].is31.addr != 0)
            return;
    }
    // all drivers uninited, release the i2c interface
    if (i2c_ready()) i2c_uninit();
}

static void init_driver(is31fl3733_driver_t *driver)
{
    if (!i2c_ready()) i2c_init();

    memset(driver->pwm_buffer, 0, PWM_BUFFER_SIZE+1);
    driver->pwm_buffer[0] = 0;
    driver->pwm_dirty           = false;
    memset(driver->control_buffer, 0, CONTROL_BUFFER_SIZE+1);
    driver->control_buffer[0]   = 0;
    driver->control_dirty       = false;

    // command register unlock
    write_register(driver->is31.addr, COMMAND_WRITE_LOCK_REG, COMMAND_UNLOCK);
    // select function page
    write_register(driver->is31.addr, COMMAND_REG, PAGE_FUNCTION);
    // reset the controller
    uint8_t reset=0;
    read_register(driver->is31.addr, FUNCTION_RESET_REG, &reset, 1);

    // command register unlock
    write_register(driver->is31.addr, COMMAND_WRITE_LOCK_REG, COMMAND_UNLOCK);
    // select control page
    write_register(driver->is31.addr, COMMAND_REG, PAGE_CONTROL);
    // turn on used leds
    for (int i = 0; i < RGB_MATRIX_LED_NUM; i++) {
        uint8_t r, g, b;
        map_led(i, &r, &g, &b);

        uint8_t reg_r = (r - 1) / 8;
        uint8_t reg_g = (g - 1) / 8;
        uint8_t reg_b = (b - 1) / 8;
        uint8_t bit_r = (r - 1) % 8;
        uint8_t bit_g = (g - 1) % 8;
        uint8_t bit_b = (b - 1) % 8;
        driver->control_buffer[reg_r + 1] |= (1 << bit_r);
        driver->control_buffer[reg_g + 1] |= (1 << bit_g);
        driver->control_buffer[reg_b + 1] |= (1 << bit_b);
    }
    i2c_send(driver->is31.addr, driver->control_buffer, CONTROL_BUFFER_SIZE+1, IS31_TIMEOUT);

    // command register unlock
    write_register(driver->is31.addr, COMMAND_WRITE_LOCK_REG, COMMAND_UNLOCK);
    // select pwm page
    write_register(driver->is31.addr, COMMAND_REG, PAGE_PWM);
    // set pwm to 0
    for (int i = 0; i <= PWM_BUFFER_SIZE; i++) {
        write_register(driver->is31.addr, i, 0);
    }

    // command register unlock
    write_register(driver->is31.addr, COMMAND_WRITE_LOCK_REG, COMMAND_UNLOCK);
    // select function page 
    write_register(driver->is31.addr, COMMAND_REG, PAGE_FUNCTION);
    // set global current
    write_register(driver->is31.addr, FUNCTION_GLOBAL_CURRENT_REG, 0xFF);
    // turn on controller
    write_register(driver->is31.addr, FUNCTION_CONFIGURATION_REG, 0x01);

    // wait the controller
    wait_ms(10);
}

static void uninit_driver(is31_t *driver)
{
    // command register unlock
    write_register(driver->addr, COMMAND_WRITE_LOCK_REG, COMMAND_UNLOCK);
    // turn off controller
    write_register(driver->addr, FUNCTION_CONFIGURATION_REG, 0);

}

void write_register(uint8_t addr, uint8_t reg, uint8_t data)
{
    i2c_write_reg(addr, reg, &data, 1, IS31_TIMEOUT);
}

static void read_register(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t size)
{
    i2c_read_reg(addr, reg, buf, 1, IS31_TIMEOUT);
}