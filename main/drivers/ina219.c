/**
 * @file ina219.c
 * 
 */

#include "ina219.h"
#include "i2c.h"
#include "amk_printf.h"

#ifndef INA219_I2C_ID
#define INA219_I2C_ID   I2C_INSTANCE_1
#endif

#define TIMEOUT 100

#ifndef INA219_DEBUG
#define INA219_DEBUG 1
#endif

#if INA219_DEBUG
#define ina219_debug  amk_printf
#else
#define ina219_debug(...)
#endif

#define INA219_CONFIG           0x00
#define INA219_SHUNT_VOLTAGE    0x01
#define INA219_BUS_VOLTAGE      0x02
#define INA219_POWER            0x03
#define INA219_CURRENT          0x04
#define INA219_CALIBRATION      0x05

static i2c_handle_t i2c_inst;

bool ina219_init(uint8_t addr, uint16_t config)
{
    if (!i2c_inst) {
        i2c_inst = i2c_init(INA219_I2C_ID);
    }

    uint32_t status = i2c_write_reg(i2c_inst, addr, INA219_CONFIG, &config, 2, TIMEOUT);
    if (status != AMK_SUCCESS) {
        ina219_debug("INA219: failed to set config: %d\n", status);
        return false;
    } else {
        ina219_debug("INA219: set config successfully: %d\n", status);
    }

    return true;
}

bool ina219_set_calibration(uint8_t addr, uint16_t calibration)
{
    uint32_t status = i2c_write_reg(i2c_inst, addr, INA219_CONFIG, &calibration, 2, TIMEOUT);
    if (status != AMK_SUCCESS) {
        ina219_debug("INA219: failed to set calibration: %d\n", status);
        return false;
    }

    return true;
}

bool ina219_get_bus_voltage(uint8_t addr, uint16_t* voltage)
{
    uint32_t status = i2c_read_reg(i2c_inst, addr, INA219_BUS_VOLTAGE, voltage, 2, TIMEOUT);
    if (status != AMK_SUCCESS) {
        ina219_debug("INA219: failed to read bus voltage: %d\n", status);
        return false;
    }

    return true;
}

bool ina219_get_shunt_voltage(uint8_t addr, uint16_t* voltage)
{
    uint32_t status = i2c_read_reg(i2c_inst, addr, INA219_SHUNT_VOLTAGE, voltage, 2, TIMEOUT);
    if (status != AMK_SUCCESS) {
        ina219_debug("INA219: failed to read shunt voltage: %d\n", status);
        return false;
    }

    return true;
}

bool ina219_get_current(uint8_t addr, uint16_t* current)
{
    uint32_t status = i2c_read_reg(i2c_inst, addr, INA219_CURRENT, current, 2, TIMEOUT);
    if (status != AMK_SUCCESS) {
        ina219_debug("INA219: failed to read current: %d\n", status);
        return false;
    }

    return true;

}

bool ina219_get_power(uint8_t addr, uint16_t* power)
{
    uint32_t status = i2c_read_reg(i2c_inst, addr, INA219_POWER, power, 2, TIMEOUT);
    if (status != AMK_SUCCESS) {
        ina219_debug("INA219: failed to read current: %d\n", status);
        return false;
    }

    return true;

}
void ina219_uninit(uint8_t addr)
{}


