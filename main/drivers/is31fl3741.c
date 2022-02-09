/**
 * @file is31fl3741.c
 * @brief driver implementation for is31fl3741
 */

#include <string.h>
#include "is31fl3741.h"
#include "rgb_common.h"
#include "i2c.h"
#include "wait.h"
#include "amk_printf.h"

#ifndef FL3741_DEBUG
#define FL3741_DEBUG 0
#endif

#if FL3741_DEBUG
#define fl3741_debug  amk_printf
#else
#define fl3741_debug(...)
#endif

#define PWM_BUFFER_SIZE     351
#define SCALE_BUFFER_SIZE   351

#define UNLOCK_REG          0xFE
#define COMMAND_REG         0xFD
#define INT_MASK_REG        0xF0
#define INT_STATUS_REG      0xF1
#define ID_REG              0xFC

#define PWM_PAGE0           0x00    // 0x00-0xB3
#define PWM_PAGE1           0x01    // 0x00-0xAA
#define SCALE_PAGE0         0x02    // 0x00-0xB3
#define SCALE_PAGE1         0x03    // 0x00-0xAA

// function page and registers
#define FUNCTION_PAGE       0x04    
#define CONFIG_REG          0x00
#define GLOBAL_CURRENT_REG  0x01
#define PD_PU_REG           0x02
#define PWM_FREQ_REG        0x36
#define RESET_REG           0x3F

#define PWM_PAGE0_SIZE      0xB4
#define PWM_PAGE1_SIZE      0xAB
#define SCALE_PAGE0_SIZE    PWM_PAGE0_SIZE
#define SCALE_PAGE1_SIZE    PWM_PAGE1_SIZE

#define UNLOCK_COMMAND      0xC5

// pullup and pulldown register for SWx and CSy
#define PD_OFFSET           0x04
#define PD_NO               0x00
#define PD_0_5K             0x01
#define PD_1K               0x02
#define PD_2K               0x03
#define PD_4K               0x04
#define PD_8K               0x05
#define PD_16K              0x06
#define PD_32K              0x07

#ifndef IS31FL3741_PD_DEFAULT
    #define IS31FL3741_PD_DEFAULT PD_32K
#endif

#define PU_OFFSET           0x00
#define PU_NO               0x00
#define PU_0_5K             0x01
#define PU_1K               0x02
#define PU_2K               0x03
#define PU_4K               0x04
#define PU_8K               0x05
#define PU_16K              0x06
#define PU_32K              0x07

#ifndef IS31FL3741_PU_DEFAULT
    #define IS31FL3741_PU_DEFAULT PU_32K
#endif

// pwm frequency
#define PWM_29K             0x00
#define PWM_3_6K            0x03
#define PWM_1_8K            0x07
#define PWM_0_9K            0x0B

// reset command
#define RESET_COMMAND       0xAE

#ifndef IS31FL3741_NUM
    #define IS31FL3741_NUM  1
#endif

#ifndef IS31FL3741_GLOBAL_CURRENT
    #define IS31FL3741_GLOBAL_CURRENT 0xFF
#endif

#define TIMEOUT             100

#ifndef IS31FL3741_I2C_ID
    #define IS31FL3741_I2C_ID   I2C_INSTANCE_1
#endif

static i2c_handle_t i2c_inst;
static bool fl3741_available = true;

typedef struct {
    i2c_led_t       i2c_led;
    uint8_t         pwm_buffer[PWM_BUFFER_SIZE];
    bool            pwm_dirty;
    uint8_t         scale_buffer[SCALE_BUFFER_SIZE];
    bool            scale_dirty;
    bool            ready;
} is31fl3741_driver_t;

static is31fl3741_driver_t is31_drivers[IS31FL3741_NUM] = {0};

static void init_driver(is31fl3741_driver_t *driver);
static void uninit_driver(i2c_led_t *driver);

bool is31fl3741_available(uint8_t addr)
{
    return fl3741_available;
}

i2c_led_t *is31fl3741_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num)
{
    is31fl3741_driver_t *driver = &is31_drivers[index];
    if (driver->ready) return &driver->i2c_led;

    driver->i2c_led.addr        = addr;
    driver->i2c_led.index       = index;
    driver->i2c_led.led_start   = led_start;
    driver->i2c_led.led_num     = led_num;
    driver->i2c_led.data        = driver;

    init_driver(driver);

    driver->ready = true;
    fl3741_debug("IS31FL3741: initialized\n");
    return &driver->i2c_led;
}

void is31fl3741_uninit(i2c_led_t *driver)
{   
    if (!is31fl3741_available(0)) return;

    // turn chip off
    uninit_driver(driver);

    // reset driver data
    is31fl3741_driver_t *is31 = (is31fl3741_driver_t*)(driver->data);
    
    memset(is31, 0, sizeof(is31fl3741_driver_t));
}

void is31fl3741_set_color(i2c_led_t *driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_led_t *led = &g_rgb_leds[index];
    is31fl3741_driver_t *is31 = (is31fl3741_driver_t*)(driver->data);
    is31->pwm_buffer[led->r] = red;
    is31->pwm_buffer[led->g] = green;
    is31->pwm_buffer[led->b] = blue;
    is31->pwm_dirty = true;
}

void is31fl3741_set_color_all(i2c_led_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < driver->led_num; i++) {
        is31fl3741_set_color(driver, i, red, green, blue);
    }
}

void is31fl3741_update_buffers(i2c_led_t *driver)
{
    if (!is31fl3741_available(0)) return;
   
    uint32_t status = AMK_SUCCESS;
    is31fl3741_driver_t *is31 = (is31fl3741_driver_t*)(driver->data);
    uint8_t data = 0;

    if (is31->pwm_dirty) {
        // select pwm page 0
        data = UNLOCK_COMMAND;
        i2c_write_reg(i2c_inst, driver->addr, UNLOCK_REG, &data, 1, TIMEOUT);
        data = PWM_PAGE0;
        i2c_write_reg(i2c_inst, driver->addr, COMMAND_REG, &data, 1, TIMEOUT);

        status = i2c_send(i2c_inst, driver->addr, is31->pwm_buffer, PWM_PAGE0_SIZE + 1, TIMEOUT);
        if (status != AMK_SUCCESS) {
            fl3741_debug("IS31FL3741: failed to update pwm page0: addr=%d, status=%d\n", driver->addr, status);
        }

        // select pwm page 1
        data = UNLOCK_COMMAND;
        i2c_write_reg(i2c_inst, driver->addr, UNLOCK_REG, &data, 1, TIMEOUT);
        data = PWM_PAGE1;
        i2c_write_reg(i2c_inst, driver->addr, COMMAND_REG, &data, 1, TIMEOUT);

        status = i2c_send(i2c_inst, driver->addr, &is31->pwm_buffer[PWM_PAGE0_SIZE+1], PWM_PAGE1_SIZE + 1, TIMEOUT);
        if (status != AMK_SUCCESS) {
            fl3741_debug("IS31FL3741: failed to update pwm page1: addr=%d, status=%d\n", driver->addr, status);
        }

        is31->pwm_dirty = false;
    }

    if (is31->scale_dirty) {
        // select scale page 0
        data = UNLOCK_COMMAND;
        i2c_write_reg(i2c_inst, driver->addr, UNLOCK_REG, &data, 1, TIMEOUT);
        data = SCALE_PAGE0;
        i2c_write_reg(i2c_inst, driver->addr, COMMAND_REG, &data, 1, TIMEOUT);

        status = i2c_send(i2c_inst, driver->addr, is31->scale_buffer, SCALE_PAGE0_SIZE + 1, TIMEOUT);
        if (status != AMK_SUCCESS) {
            fl3741_debug("IS31FL3741: failed to update scale page0: addr=%d, status=%d\n", driver->addr, status);
        }

        // select scale page 1
        data = UNLOCK_COMMAND;
        i2c_write_reg(i2c_inst, driver->addr, UNLOCK_REG, &data, 1, TIMEOUT);
        data = SCALE_PAGE1;
        i2c_write_reg(i2c_inst, driver->addr, COMMAND_REG, &data, 1, TIMEOUT);

        status = i2c_send(i2c_inst, driver->addr, &is31->scale_buffer[SCALE_PAGE0_SIZE+1], SCALE_PAGE1_SIZE + 1, TIMEOUT);
        if (status != AMK_SUCCESS) {
            fl3741_debug("IS31FL3741: failed to update scale page1: addr=%d, status=%d\n", driver->addr, status);
        }

        is31->scale_dirty = false;
    }
}


void init_driver(is31fl3741_driver_t *driver)
{
    uint32_t status = AMK_SUCCESS;
    if (!i2c_inst) {
        i2c_inst = i2c_init(IS31FL3741_I2C_ID);
    }

    // select function page
    uint8_t data = UNLOCK_COMMAND;
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, UNLOCK_REG, &data, 1, TIMEOUT);
    data = FUNCTION_PAGE;
    i2c_write_reg(i2c_inst, driver->i2c_led.addr, COMMAND_REG, &data, 1, TIMEOUT);

    // Reset 3741 to default state
    data = RESET_COMMAND;
    status = i2c_write_reg(i2c_inst, driver->i2c_led.addr, RESET_REG, &data, 1, TIMEOUT);

    if (status != AMK_SUCCESS) {
        fl3741_debug("IS31FL3741: failed to reset: %d\n", status);
        fl3741_available = false;
        return;
    }
    wait_ms(10);

    // enable chip
    data = 0x01;
    status = i2c_write_reg(i2c_inst, driver->i2c_led.addr, CONFIG_REG, &data, 1, TIMEOUT);

    // set global current
    data = IS31FL3741_GLOBAL_CURRENT;
    status = i2c_write_reg(i2c_inst, driver->i2c_led.addr, GLOBAL_CURRENT_REG, &data, 1, TIMEOUT);

    // set pulldown and pullup
    data = ((IS31FL3741_PD_DEFAULT<<PD_OFFSET) | (IS31FL3741_PU_DEFAULT<<PU_OFFSET));
    status = i2c_write_reg(i2c_inst, driver->i2c_led.addr, PD_PU_REG, &data, 1, TIMEOUT);

    // reset pwm and scale buffers 
    memset(driver->pwm_buffer, 0, PWM_BUFFER_SIZE);
    driver->pwm_dirty = true;

    memset(driver->scale_buffer, 0xFF, SCALE_BUFFER_SIZE);
    driver->scale_dirty = true;

    is31fl3741_update_buffers(&driver->i2c_led);
}

void uninit_driver(i2c_led_t *driver)
{
    // shutdonw driver
    uint8_t data = UNLOCK_COMMAND;
    i2c_write_reg(i2c_inst, driver->addr, UNLOCK_REG, &data, 1, TIMEOUT);
    data = FUNCTION_PAGE;
    i2c_write_reg(i2c_inst, driver->addr, COMMAND_REG, &data, 1, TIMEOUT);
    data = 0;
    i2c_write_reg(i2c_inst, driver->addr, COMMAND_REG, &data, 1, TIMEOUT);
}
