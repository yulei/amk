/**
 * @file mcp4017.c
 * @author astro
 * 
 * 
 * @copyright Copyright (c) 2024
 */

#include "digital_pot.h"
#include "i2c.h"
#include "amk_utils.h"
#include "amk_printf.h"

#ifndef MCP4017_DEBUG
#define MCP4017_DEBUG 1
#endif

#if MCP4017_DEBUG
#define mcp4017_debug  amk_printf
#else
#define mcp4017_debug
#endif

#define MCP4017_ADDR        0b01011110
#ifndef MCP4017_I2C_ID
#define MCP4017_I2C_ID     I2C_INSTANCE_1
#endif

#define MCP4017_TIMEOUT     100

static i2c_handle_t i2c_inst;

bool digital_pot_init(void)
{
    if (!i2c_inst) {
        i2c_inst = ak_i2c_init(MCP4017_I2C_ID);
    }

    uint8_t data = 0;
    if ( AMK_SUCCESS == ak_i2c_recv(i2c_inst, MCP4017_ADDR, &data, 1, MCP4017_TIMEOUT)) {
        mcp4017_debug("MCP4017 current index: %d\n", data);
        return true;
    } else {
        mcp4017_debug("MCP4017 failed to initialized\n");
    } 

    return false;
}

bool digital_pot_set(uint8_t index)
{
    uint8_t data = index;
    if ( AMK_SUCCESS == ak_i2c_send(i2c_inst, MCP4017_ADDR, &data, 1, MCP4017_TIMEOUT)) {
        mcp4017_debug("MCP4017 set index: %d\n", data);
        return true;
    } else {
        mcp4017_debug("MCP4017 failed to set index at %d\n", index);
    } 
    
    return false;
}

void digital_uninit(void)
{
    // do nothing
}
