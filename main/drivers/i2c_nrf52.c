 /**
  * i2c_nrf52.c
  */

#include <stdbool.h>
#include <string.h>

#include "i2c.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrfx_twi.h"
#include "nrf_log.h"
#include "nrf_gpio.h"
#include "rf_power.h"
#include "amk_printf.h"

#ifndef NRF52_I2C_DEBUG
#define NRF52_I2C_DEBUG 1
#endif

#if NRF52_I2C_DEBUG
#define nrf52_i2c_debug  amk_printf
#else
#define nrf52_i2c_debug(...)
#endif

typedef struct {
    nrfx_twi_t  twi;
    uint32_t    scl;
    uint32_t    sda;
    uint32_t    id;
    uint32_t    error;
    bool        ready;
} i2c_instance_t;

#ifdef USE_I2C1
    #define I2C1_ID     0
    #define I2C1_SCL    I2C1_SCL_PIN
    #define I2C1_SDA    I2C1_SDA_PIN
    static volatile bool done1 = false;
    static i2c_instance_t m_i2c1 = {
        .twi = NRFX_TWI_INSTANCE(I2C1_ID),
        .scl = I2C1_SCL,
        .sda = I2C1_SDA,
        .id = I2C_INSTANCE_1,
        .error = AMK_SUCCESS,
        .ready = false,
    };
#endif


#define TWI_ADDR(addr) (((addr)>>1)&0x7F)

static void set_done(i2c_instance_t *inst, bool state)
{
    if (inst->id == I2C_INSTANCE_1) {
        done1 = state;
    }
}

static void wait_done(i2c_instance_t *inst)
{
    if (inst->id == I2C_INSTANCE_1) {
        while( !done1) {
        };
    }
}

static void i2c_event_handler(nrfx_twi_evt_t const *p_event, void *p_context);

static void i2c_prepare_sleep(void *context)
{
    i2c_instance_t *inst = (i2c_instance_t*)context;
    i2c_uninit(inst);
}

static void i2c_inst_init(i2c_instance_t *inst)
{
    if (i2c_ready(inst))
        return;

    ret_code_t err_code = NRFX_SUCCESS;

    nrfx_twi_config_t config = NRFX_TWI_DEFAULT_CONFIG;
    config.scl = inst->scl;
    config.sda = inst->sda;

    err_code = nrfx_twi_init(&inst->twi, &config, i2c_event_handler, inst);
    APP_ERROR_CHECK(err_code);
    inst->ready = true;
    nrfx_twi_enable(&inst->twi);
    //NRF_LOG_INFO("twi enabled");
    nrf52_i2c_debug("twi enabled\n");
    rf_power_register(i2c_prepare_sleep, inst);
}

static void i2c_event_handler(nrfx_twi_evt_t const *p_event, void *p_context)
{
    i2c_instance_t *inst = (i2c_instance_t*)p_context;
    switch( p_event->type) {
    case NRFX_TWI_EVT_DONE:
        //NRF_LOG_INFO("i2c event done");
        inst->error = AMK_SUCCESS;
        //error1 = AMK_SUCCESS;
        break;
    case NRFX_TWI_EVT_ADDRESS_NACK:
        nrf52_i2c_debug("i2c address nack\n");
        inst->error = AMK_I2C_ERROR;
        //error1 = AMK_I2C_ERROR;
        break;
    case NRFX_TWI_EVT_DATA_NACK:
        nrf52_i2c_debug("i2c data nack\n");
        inst->error = AMK_I2C_ERROR;
        //error1 = AMK_I2C_ERROR;
        break;
    case NRFX_TWI_EVT_OVERRUN:
        nrf52_i2c_debug("i2c overrun");
        inst->error = AMK_I2C_ERROR;
        //error1 = AMK_I2C_ERROR;
        break;
    case NRFX_TWI_EVT_BUS_ERROR:
        nrf52_i2c_debug("i2c bus error");
        inst->error = AMK_I2C_ERROR;
        //error1 = AMK_I2C_ERROR;
        break;
    }
    set_done(inst, true);
}

i2c_handle_t i2c_init(I2C_ID id)
{
#ifdef USE_I2C1
    if (id == I2C_INSTANCE_1) {
        i2c_inst_init(&m_i2c1);
        return &m_i2c1;
    }
#endif
    return NULL;
}

bool i2c_ready(i2c_handle_t i2c)
{
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    return inst->ready;
}

amk_error_t i2c_send(i2c_handle_t i2c, uint8_t addr, const void *data, size_t length, size_t timeout)
{
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    (void)timeout;
    inst->error = AMK_SUCCESS;
    set_done(inst, false);
    ret_code_t err_code = nrfx_twi_tx(&inst->twi, TWI_ADDR(addr), data, length, false);

    if (err_code != NRFX_SUCCESS) {
        nrf52_i2c_debug("twi TX error: %d\n", err_code);
        return AMK_I2C_ERROR;
    }

    wait_done(inst);

    return inst->error;
}

amk_error_t i2c_recv(i2c_handle_t i2c, uint8_t addr, void* data, size_t length, size_t timeout)
{
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    (void)timeout;
    inst->error = AMK_SUCCESS;
    set_done(inst, false);
    ret_code_t err_code = nrfx_twi_rx(&inst->twi, TWI_ADDR(addr), data, length);
    if (err_code != NRFX_SUCCESS) {
        nrf52_i2c_debug("twi RX error: %d\n", err_code);
        return AMK_I2C_ERROR;
    }

    wait_done(inst);

    return inst->error;
}

amk_error_t i2c_write_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, const void* data, size_t length, size_t timeout)
{
    (void)timeout;
    uint8_t packet[length + 1];
    memcpy(&packet[1], data, length);
    packet[0] = reg;
    return i2c_send(i2c, addr, packet, length + 1, 0);
}

amk_error_t i2c_read_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, void* data, size_t length, size_t timeout)
{
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    (void)timeout;
    set_done(inst, false);
    inst->error = AMK_SUCCESS;
    nrfx_twi_xfer_desc_t txrx = NRFX_TWI_XFER_DESC_TXRX(TWI_ADDR(addr), &reg, 1, data, length);
    ret_code_t err_code = nrfx_twi_xfer(&inst->twi, &txrx, 0);
    if (err_code != NRFX_SUCCESS) {
        nrf52_i2c_debug("twi XFER error: %d\n", err_code);
        return AMK_I2C_ERROR;
    }

    wait_done(inst);

    return inst->error;
}

void i2c_uninit(i2c_handle_t i2c)
{
    if (!i2c_ready(i2c)) return;

    i2c_instance_t *inst = (i2c_instance_t*)i2c;

    //nrfx_twi_disable(&inst->twi);

    nrfx_twi_uninit(&inst->twi);

    nrf52_i2c_debug("twi disabled\n");
    inst->ready = false;
}