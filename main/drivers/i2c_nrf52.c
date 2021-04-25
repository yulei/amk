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
#include "rf_power.h"

#ifndef I2C_INSTANCE_ID
    #define I2C_INSTANCE_ID     0
#endif

#ifndef I2C_SCL_PIN
    #define I2C_SCL_PIN         18
#endif

#ifndef I2C_SDA_PIN
    #define I2C_SDA_PIN         19
#endif

/* TWI instance. */
static const nrfx_twi_t m_twi = NRFX_TWI_INSTANCE(I2C_INSTANCE_ID);
static bool twi_ready = false;

#define TWI_ADDR(addr) ((addr)>>1)

static void i2c_prepare_sleep(void* context)
{
    (void)(context);
    i2c_uninit();
}

bool i2c_ready(void) { return twi_ready; }

void i2c_init(void)
{
    if (i2c_ready())
        return;
    ret_code_t err_code = NRFX_SUCCESS;

    nrfx_twi_config_t twi_config = NRFX_TWI_DEFAULT_CONFIG;
    twi_config.scl = I2C_SCL_PIN;
    twi_config.sda = I2C_SDA_PIN;

    err_code = nrfx_twi_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);
    twi_ready = true;
    nrfx_twi_enable(&m_twi);
    NRF_LOG_INFO("twi enabled");
    rf_power_register(i2c_prepare_sleep, NULL);
}

amk_error_t i2c_send(uint8_t addr, const void* data, size_t length, size_t timeout)
{
    (void)timeout;
    ret_code_t err_code = nrfx_twi_tx(&m_twi, TWI_ADDR(addr), data, length, false);
    if (err_code != NRFX_SUCCESS) {
        return AMK_I2C_ERROR;
    }
    return AMK_SUCCESS;
}

amk_error_t i2c_recv(uint8_t addr, void* data, size_t length, size_t timeout)
{
    (void)timeout;
    ret_code_t err_code = nrfx_twi_rx(&m_twi, TWI_ADDR(addr), data, length);
    if (err_code != NRFX_SUCCESS) {
        return AMK_I2C_ERROR;
    }
    return AMK_SUCCESS;
}

amk_error_t i2c_write_reg(uint8_t addr, uint8_t reg, const void* data, size_t length, size_t timeout)
{
    (void)timeout;
    uint8_t packet[length + 1];
    memcpy(&packet[1], data, length);
    packet[0] = reg;
    return i2c_send(addr, packet, length + 1, 0);
}

amk_error_t i2c_read_reg(uint8_t addr, uint8_t reg, void* data, size_t length, size_t timeout)
{
    (void)timeout;
    nrfx_twi_xfer_desc_t txrx = NRFX_TWI_XFER_DESC_TXRX(TWI_ADDR(addr), &reg, 1, data, length);
    ret_code_t err_code = nrfx_twi_xfer(&m_twi, &txrx, 0);
    if (err_code != NRFX_SUCCESS) {
        return AMK_I2C_ERROR;
    }
    return AMK_SUCCESS;
}

void i2c_uninit(void)
{
    if (!i2c_ready()) return;

    //nrfx_twi_disable(&m_twi);

    nrfx_twi_uninit(&m_twi);

    NRF_LOG_INFO("twi disabled");
    twi_ready = false;
}