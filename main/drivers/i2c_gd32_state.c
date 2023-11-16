/**
 * @file i2c_gd32_state.c
 * @author astro
 * @brief state machine based
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdbool.h>
#include <string.h>

#include "amk_hal.h"
#include "timer.h"
#include "i2c.h"
#include "amk_gpio.h"
#include "amk_printf.h"

#ifndef I2C_DEBUG
#define I2C_DEBUG 1
#endif

#if I2C_DEBUG
#define i2c_debug  amk_printf
#else
#define i2c_debug(...)
#endif

typedef struct {
    uint32_t handle;
    bool ready;
} i2c_instance_t;

enum
{
    I2C_START = 0,
    I2C_SEND_ADDRESS,
    I2C_RESTART,
    I2C_TRANSMIT_DATA,
    I2C_STOP
};

#ifdef USE_I2C1
    #ifndef I2C1_ID
    #define I2C1_ID     I2C0 
    #endif

    #ifndef I2C1_PINS
    #define I2C1_PINS   (GPIO_PIN_6 | GPIO_PIN_7)
    #endif
    #ifndef I2C1_SCL_PIN
    #define I2C1_SCL_PIN GPIO_PIN_6
    #endif

    #ifndef I2C1_SDA_PIN
    #define I2C1_SDA_PIN GPIO_PIN_7
    #endif
    static i2c_instance_t m_i2c1 = {
        .ready = false,
    };
    static void i2c1_config(void)
    {
        rcu_periph_clock_enable(RCU_GPIOB);
        rcu_periph_clock_enable(RCU_I2C0);
        rcu_periph_clock_enable(RCU_AF);

        i2c_deinit(I2C0);

        gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C1_PINS);
        #ifdef I2C1_REMAP
        gpio_pin_remap_config(GPIO_I2C0_REMAP, ENABLE);
        #endif

        i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
        /* I2C address configure */
        i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x70);
        /* enable I2C0 */
        i2c_enable(I2C0);
        /* enable acknowledge */
        i2c_ack_config(I2C0, I2C_ACK_ENABLE);
    }
#endif

static void i2c_bus_reset(void)
{
    /* configure SDA/SCL for GPIO */
    GPIO_BC(GPIOB) |= I2C1_PINS;
    gpio_init(GPIOB,GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, I2C1_PINS);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    /* stop signal */
    GPIO_BOP(GPIOB) |= I2C1_SCL_PIN;
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    GPIO_BOP(GPIOB) |= I2C1_SDA_PIN;

#ifdef USE_I2C1
    i2c1_config();
#endif
}

bool i2c_ready(i2c_handle_t i2c)
{
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    return inst->ready;
}

void i2c_inst_init(i2c_instance_t *inst, uint32_t i2c)
{
    if (i2c_ready(inst))
        return;
#ifdef USE_I2C1
    if (i2c == I2C1_ID) {
        //i2c1_config();
        i2c_bus_reset();
        inst->handle = i2c;
        inst->ready = true;
    }
#endif
}

i2c_handle_t i2c_init(I2C_ID id)
{
#ifdef USE_I2C1
    if (id == I2C_INSTANCE_1) {
        i2c_inst_init(&m_i2c1, I2C1_ID);
        return &m_i2c1;
    }
#endif

#ifdef USE_I2C2 
    if (id == I2C_INSTANCE_2) {
        i2c_inst_init(&m_i2c2, I2C2_ID);
        return &m_i2c2;
    }
#endif

    return NULL;
}

FlagStatus i2c_flag_get(uint32_t i2c_periph, i2c_flag_enum flag);

bool i2c_wait_flag_timeout(uint32_t i2c, i2c_flag_enum flag, FlagStatus status, uint32_t timeout, uint32_t start)
{
    while(timer_elapsed32(start) < timeout) {
        if (i2c_flag_get(i2c, flag) == status) {
            return false;
        }
    }
    return true;
}

amk_error_t i2c_send(i2c_handle_t i2c, uint8_t addr, const void* data, size_t length, size_t timeout)
{
    uint32_t start = timer_read32();
    uint32_t state = I2C_START;
    bool quit = false;
    amk_error_t error = AMK_SUCCESS;

    while (!quit) {
        switch(state) {
        case I2C_START:
            if (i2c_wait_flag_timeout(I2C0, I2C_FLAG_I2CBSY, RESET, timeout, start)) {
                error = AMK_I2C_TIMEOUT;
                i2c_debug("I2C GD32: i2c_send: BUS Busy timeout\n");
            } else {
                i2c_start_on_bus(I2C0);
                if (i2c_wait_flag_timeout(I2C0, I2C_FLAG_SBSEND, SET, timeout, start)) {
                    error = AMK_I2C_TIMEOUT;
                    i2c_debug("I2C GD32: i2c_send: SBSSEND timeout\n");
                } else {
                    state = I2C_SEND_ADDRESS; 
                }
            }
        break;
        case I2C_SEND_ADDRESS:
            i2c_master_addressing(I2C0, addr, I2C_TRANSMITTER);
            if (i2c_wait_flag_timeout(I2C0, I2C_FLAG_ADDSEND, SET, timeout, start)) {
                error = AMK_I2C_TIMEOUT;
                i2c_debug("I2C GD32: i2c_send: ADDSEND timeout\n");
            } else {
                i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
                state = I2C_TRANSMIT_DATA;
            }
        break;
        case I2C_TRANSMIT_DATA:
            {
                uint8_t *p = (uint8_t *)data;
                for(int i = 0; i < length; i++) {
                    if(i2c_wait_flag_timeout(I2C0, I2C_FLAG_TBE, SET, timeout, start)) {
                        error = AMK_I2C_TIMEOUT;
                        i2c_debug("I2C GD32: i2c_send: TBE timeout, breakout\n");
                        break;
                    } 

                    i2c_data_transmit(I2C0, p[i]);
                }

                if(i2c_wait_flag_timeout(I2C0, I2C_FLAG_TBE, SET, timeout, start)) {
                    error = AMK_I2C_TIMEOUT;
                    i2c_debug("I2C GD32: i2c_send: TBE timeout\n");
                } else {
                    state = I2C_STOP;
                }
            }
        break;
        case I2C_STOP:
            i2c_stop_on_bus(I2C0);
            while(I2C_CTL0(I2C0)&0x0200) {
                if (timer_elapsed32(start)  > timeout) {
                    error = AMK_I2C_TIMEOUT;
                    i2c_debug("I2C GD32: i2c_send: STOP timeout\n");
                    break;
                }
            }
            quit = true;
        break;
        default:
            // never here
            quit = true;
            i2c_debug("I2C GD32: i2c_send: default state\n");
        break;
        }

        if (error != AMK_SUCCESS) {
            i2c_debug("I2C GD32: i2c_send: error=%d\n", error);
            break;
        }

        if (timer_elapsed32(start) > timeout) {
            error = AMK_I2C_TIMEOUT;
            i2c_debug("I2C GD32: i2c_send: total timeout\n");
            quit = true;
        }
    }

    if (error != AMK_SUCCESS) {
        i2c_bus_reset();
        i2c_debug("I2C GD32: i2c_send: bus reset because error\n");
    }

    return error;
}

amk_error_t i2c_recv(i2c_handle_t i2c, uint8_t addr, void* data, size_t length, size_t timeout)
{
    return AMK_ERROR;
}

amk_error_t i2c_write_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, const void* data, size_t length, size_t timeout)
{
    uint32_t start = timer_read32();
    uint32_t state = I2C_START;
    bool quit = false;
    amk_error_t error = AMK_SUCCESS;

    while (!quit) {
        switch(state) {
        case I2C_START:
            if (i2c_wait_flag_timeout(I2C0, I2C_FLAG_I2CBSY, RESET, timeout, start)) {
                error = AMK_I2C_TIMEOUT;
                i2c_debug("I2C GD32: i2c_write_reg: BUS Busy timeout\n");
            } else {
                i2c_start_on_bus(I2C0);
                if (i2c_wait_flag_timeout(I2C0, I2C_FLAG_SBSEND, SET, timeout, start)) {
                    error = AMK_I2C_TIMEOUT;
                    i2c_debug("I2C GD32: i2c_write_reg: SBSSEND timeout\n");
                } else {
                    state = I2C_SEND_ADDRESS; 
                }
            }
        break;
        case I2C_SEND_ADDRESS:
            i2c_master_addressing(I2C0, addr, I2C_TRANSMITTER);
            if (i2c_wait_flag_timeout(I2C0, I2C_FLAG_ADDSEND, SET, timeout, start)) {
                error = AMK_I2C_TIMEOUT;
                i2c_debug("I2C GD32: i2c_write_reg: ADDSEND timeout\n");
                //state = I2C_START;
            } else {
                i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
                state = I2C_TRANSMIT_DATA;
            }
        break;
        case I2C_TRANSMIT_DATA:
            {   
                uint8_t *p = (uint8_t *)data;
                for(int i = 0; i < length+1; i++) {
                    if(i2c_wait_flag_timeout(I2C0, I2C_FLAG_TBE, SET, timeout, start)) {
                        error = AMK_I2C_TIMEOUT;
                        i2c_debug("I2C GD32: i2c_write_reg: TBE timeout, index=%d, breakout\n", i);
                        break;
                    } 

                    // transmit register first
                    if (i == 0) {
                        i2c_data_transmit(I2C0, reg);
                    } else {
                        i2c_data_transmit(I2C0, p[i-1]);
                    }
                }

                if(i2c_wait_flag_timeout(I2C0, I2C_FLAG_TBE, SET, timeout, start)) {
                    error = AMK_I2C_TIMEOUT;
                    i2c_debug("I2C GD32: i2c_write_reg: TBE timeout\n");
                } else {
                    state = I2C_STOP;
                }
            }
        break;
        case I2C_STOP:
            i2c_stop_on_bus(I2C0);
            while(I2C_CTL0(I2C0)&0x0200) {
                if (timer_elapsed32(start)  > timeout) {
                    error = AMK_I2C_TIMEOUT;
                    i2c_debug("I2C GD32: i2c_write_reg: STOP timeout\n");
                    break;
                }
            }
            quit = true;
        break;
        default:
            // never here
            quit = true;
            i2c_debug("I2C GD32: i2c_write_reg: default state\n");
        break;
        }

        if (error != AMK_SUCCESS) {
            break;
        }

        if (timer_elapsed32(start) > timeout) {
            error = AMK_I2C_TIMEOUT;
            i2c_debug("I2C GD32: i2c_write_reg: total timeout\n");
            quit = true;
        }
    }

    if (error != AMK_SUCCESS) {
        i2c_bus_reset();
        i2c_debug("I2C GD32: i2c_write_reg: bus reset because error\n");
    }

    return error;
}

amk_error_t i2c_read_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, void* data, size_t length, size_t timeout)
{
    return AMK_SUCCESS;
}

void i2c_uninit(i2c_handle_t i2c)
{
    //i2c_instance_t *inst = (i2c_instance_t*)i2c;
}

amk_error_t i2c_send_async(i2c_handle_t i2c, uint8_t addr, const void *data, size_t length)
{
    return AMK_ERROR;
}
