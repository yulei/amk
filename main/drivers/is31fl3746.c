/**
 * @file is31fl3746.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <string.h>
#include "is31fl3746.h"
#include "rgb_common.h"
#include "i2c.h"
#include "wait.h"
#include "amk_printf.h"

#ifndef FL3746_DEBUG
#define FL3746_DEBUG 1
#endif

#if FL3746_DEBUG
#define fl3746_debug  amk_printf
#else
#define fl3746_debug(...)
#endif

#define PWM_BUFFER_SIZE     0x48
#define SCALE_BUFFER_SIZE   0x48

#define UNLOCK_COMMAND      0xC5

#define UNLOCK_REG          0xFE
#define COMMAND_REG         0xFD
#define ID_REG              0xFC

#define PWM_PAGE            0x00    // 0x01-0x48 pwm value
#define FUNC_PAGE           0x01    // 0x01-0x48 scaling value, 0x50-0x6F configuration

// registers 
#define CONFIG_REG          0x50
#define GLOBAL_CURRENT_REG  0x51
#define PD_PU_REG           0x52
// 0x53-0x5E, open/short
// 0x5F, temperature
// 0x60, spread spectrum
#define RESET_REG           0x8F
#define RESET_COMMAND       0xAE

#define PWM_FREQ_ENR_EG     0xE0
#define PWM_FREQ_REG        0xE2

// pullup and pulldown register for SWx and CSy
#define PD_OFFSET           0x04
#define PD_NO               0x00
#define PD_0_5K_OFF         0x01
#define PD_1K_OFF           0x02
#define PD_2K_OFF           0x03
#define PD_1K_ALL           0x04
#define PD_2K_ALL           0x05
#define PD_4K_ALL           0x06
#define PD_8K_ALL           0x07

#ifndef IS31FL3746_PD_DEFAULT
    #define IS31FL3746_PD_DEFAULT PD_NO
#endif

#define PU_OFFSET           0x00
#define PU_NO               0x00
#define PU_0_5K_OFF         0x01
#define PU_1K_OFF           0x02
#define PU_2K_OFF           0x03
#define PU_1K_ALL           0x04
#define PU_2K_ALL           0x05
#define PU_4K_ALL           0x06
#define PU_8K_ALL           0x07

#ifndef IS31FL3746_PU_DEFAULT
    #define IS31FL3746_PU_DEFAULT PU_NO
#endif

// pwm frequency
#define PWM_29K             0x00
#define PWM_14_5K           0x10
#define PWM_7_25K           0x20
#define PWM_3_63K           0x30
#define PWM_1_81K           0x40
#define PWM_906             0x50
#define PWM_453             0x60

// reset command
#define RESET_COMMAND       0xAE

#ifndef IS31FL3746_NUM
    #define IS31FL3746_NUM  1
#endif

#ifndef IS31FL3746_GLOBAL_CURRENT
    #define IS31FL3746_GLOBAL_CURRENT 0xFF
#endif

#define TIMEOUT             100

#ifndef IS31FL3746_I2C_ID
    #define IS31FL3746_I2C_ID   I2C_INSTANCE_1
#endif

static i2c_handle_t i2c_inst;
static bool fl3746_available = true;

typedef struct {
    i2c_led_t       i2c_led;
    uint8_t         pwm_buffer[PWM_BUFFER_SIZE+1];
    bool            pwm_dirty;
    uint8_t         scale_buffer[SCALE_BUFFER_SIZE+1];
    bool            scale_dirty;
    bool            ready;
} is31fl3746_driver_t;

static is31fl3746_driver_t is31_drivers[IS31FL3746_NUM] = {0};

static void init_driver(is31fl3746_driver_t *driver);
static void uninit_driver(i2c_led_t *driver);

bool is31fl3746_available(uint8_t addr)
{
    return fl3746_available;
}

i2c_led_t *is31fl3746_init(uint8_t addr, uint8_t index, uint8_t led_start, uint8_t led_num)
{
    is31fl3746_driver_t *driver = &is31_drivers[index];
    if (driver->ready) return &driver->i2c_led;

    driver->i2c_led.addr        = addr;
    driver->i2c_led.index       = index;
    driver->i2c_led.led_start   = led_start;
    driver->i2c_led.led_num     = led_num;
    driver->i2c_led.data        = driver;

    init_driver(driver);

    driver->ready = true;
    fl3746_debug("IS31FL3746: initialized\n");
    return &driver->i2c_led;
}

void is31fl3746_uninit(i2c_led_t *driver)
{   
    if (!is31fl3746_available(0)) return;

    // turn chip off
    uninit_driver(driver);

    // reset driver data
    is31fl3746_driver_t *is31 = (is31fl3746_driver_t*)(driver->data);
    
    memset(is31, 0, sizeof(is31fl3746_driver_t));
}

void is31fl3746_set_color(i2c_led_t *driver, uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_led_t *led = &g_rgb_leds[index];
    is31fl3746_driver_t *is31 = (is31fl3746_driver_t*)(driver->data);

    is31->pwm_buffer[led->r] = red;
    is31->pwm_buffer[led->g] = green;
    is31->pwm_buffer[led->b] = blue;
    is31->pwm_dirty = true;
}

void is31fl3746_set_color_all(i2c_led_t *driver, uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < driver->led_num; i++) {
        is31fl3746_set_color(driver, i, red, green, blue);
    }
}

static void is31fl3746_update_pwm_buffers(i2c_led_t *driver)
{
    uint32_t status = AMK_SUCCESS;
    is31fl3746_driver_t *is31 = (is31fl3746_driver_t*)(driver->data);
    if (!is31->pwm_dirty) return;

    if (!ak_i2c_ready(i2c_inst)) return;

    // select pwm page 0
    uint8_t data = UNLOCK_COMMAND;
    ak_i2c_write_reg(i2c_inst, driver->addr, UNLOCK_REG, &data, 1, TIMEOUT);
    data = PWM_PAGE;
    ak_i2c_write_reg(i2c_inst, driver->addr, COMMAND_REG, &data, 1, TIMEOUT);

    is31->pwm_buffer[0] = 1;
    status = ak_i2c_send(i2c_inst, driver->addr, is31->pwm_buffer, PWM_BUFFER_SIZE+1, TIMEOUT);
    //status = ak_i2c_send_async(i2c_inst, driver->addr, is31->pwm_buffer, PWM_BUFFER_SIZE+1);

    if (status != AMK_SUCCESS) {
        fl3746_debug("IS31FL3746: failed to update pwm page0: addr=%d, status=%d\n", driver->addr, status);
    }

    is31->pwm_dirty = false;
}

void is31fl3746_update_scale_buffers(i2c_led_t *driver)
{
    if (!is31fl3746_available(0)) return;
   
    uint32_t status = AMK_SUCCESS;
    is31fl3746_driver_t *is31 = (is31fl3746_driver_t*)(driver->data);

    if (is31->scale_dirty) {
        // select function page
        uint8_t data = UNLOCK_COMMAND;
        ak_i2c_write_reg(i2c_inst, driver->addr, UNLOCK_REG, &data, 1, TIMEOUT);
        data = FUNC_PAGE;
        ak_i2c_write_reg(i2c_inst, driver->addr, COMMAND_REG, &data, 1, TIMEOUT);

        is31->scale_buffer[0] = 1;
        status = ak_i2c_send(i2c_inst, driver->addr, is31->scale_buffer, SCALE_BUFFER_SIZE+1, TIMEOUT);

        if (status != AMK_SUCCESS) {
            fl3746_debug("IS31FL3746: failed to update scale page0: addr=%d, status=%d\n", driver->addr, status);
        }

        is31->scale_dirty = false;
    }
}

void init_driver(is31fl3746_driver_t *driver)
{
    uint32_t status = AMK_SUCCESS;
    if (!i2c_inst) {
        i2c_inst = ak_i2c_init(IS31FL3746_I2C_ID);
    }

    // select function page
    uint8_t data = UNLOCK_COMMAND;
    ak_i2c_write_reg(i2c_inst, driver->i2c_led.addr, UNLOCK_REG, &data, 1, TIMEOUT);
    data = FUNC_PAGE;
    ak_i2c_write_reg(i2c_inst, driver->i2c_led.addr, COMMAND_REG, &data, 1, TIMEOUT);

    // Reset 3746 to default state
    data = RESET_COMMAND;
    status = ak_i2c_write_reg(i2c_inst, driver->i2c_led.addr, RESET_REG, &data, 1, TIMEOUT);

    if (status != AMK_SUCCESS) {
        fl3746_debug("IS31FL3746: failed to reset: %d\n", status);
        fl3746_available = false;
        return;
    }

    wait_ms(10);

    // enable chip
    data = 0x01;
    status = ak_i2c_write_reg(i2c_inst, driver->i2c_led.addr, CONFIG_REG, &data, 1, TIMEOUT);

    // set global current
    data = 0x80;//IS31FL3746_GLOBAL_CURRENT;
    status = ak_i2c_write_reg(i2c_inst, driver->i2c_led.addr, GLOBAL_CURRENT_REG, &data, 1, TIMEOUT);

    // set pulldown and pullup
    data = ((IS31FL3746_PD_DEFAULT<<PD_OFFSET) | (IS31FL3746_PU_DEFAULT<<PU_OFFSET));
    status = ak_i2c_write_reg(i2c_inst, driver->i2c_led.addr, PD_PU_REG, &data, 1, TIMEOUT);

    // reset pwm and scale buffers 
    memset(driver->pwm_buffer, 0, PWM_BUFFER_SIZE+1);
    driver->pwm_dirty = true;

    memset(driver->scale_buffer, 0x40, SCALE_BUFFER_SIZE+1);
    driver->scale_dirty = true;

    is31fl3746_update_scale_buffers(&driver->i2c_led);
}

void is31fl3746_update_buffers(i2c_led_t *driver)
{
    is31fl3746_update_pwm_buffers(driver);
}

void uninit_driver(i2c_led_t *driver)
{
    // shutdonw driver
    uint8_t data = UNLOCK_COMMAND;
    ak_i2c_write_reg(i2c_inst, driver->addr, UNLOCK_REG, &data, 1, TIMEOUT);
    data = FUNC_PAGE;
    ak_i2c_write_reg(i2c_inst, driver->addr, COMMAND_REG, &data, 1, TIMEOUT);
    data = 0;
    ak_i2c_write_reg(i2c_inst, driver->addr, CONFIG_REG, &data, 1, TIMEOUT);
}
