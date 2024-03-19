/**
 * @file i2c_gd32_dma.c
 * @author astro
 * @brief state machine with dma
 * 
 * @copyright Copyright (c) 2023
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
    bool busy;
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
        .busy = false,
        .ready = false,
    };
    static void i2c1_config(void)
    {
        rcu_periph_clock_enable(RCU_GPIOB);
        rcu_periph_clock_enable(RCU_I2C0);
        rcu_periph_clock_enable(RCU_AF);
        rcu_periph_clock_enable(RCU_DMA0);

        i2c_deinit(I2C0);

        #ifdef I2C1_REMAP
        gpio_pin_remap_config(GPIO_I2C0_REMAP, ENABLE);
        #endif

        gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C1_PINS);

        i2c_clock_config(I2C0, 400000, I2C_DTCY_2);
        /* I2C address configure */
        i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x70);
        /* enable I2C0 */
        i2c_enable(I2C0);
        /* enable acknowledge */
        i2c_ack_config(I2C0, I2C_ACK_ENABLE);

        /* initialize DMA channel 5 (i2c0 tx) */
        dma_parameter_struct dma_init_struct;
        dma_deinit(DMA0, DMA_CH5);
        dma_struct_para_init(&dma_init_struct);

        dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
        dma_init_struct.periph_addr = (uint32_t)&I2C_DATA(I2C1);
        dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.periph_addr = (uint32_t)&I2C_DATA(I2C0);
        dma_init_struct.priority = DMA_PRIORITY_HIGH;
        dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        //dma_init_struct.memory_addr = (uint32_t)i2c0_buff_tx;
        //dma_init_struct.number = BUFFER_SIZE;
        dma_init(DMA0, DMA_CH5, &dma_init_struct);
    }
#endif

#ifdef USE_I2C1
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

    i2c1_config();
}
#endif

bool ak_i2c_ready(i2c_handle_t i2c)
{
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    return inst->ready;
}

void i2c_inst_init(i2c_instance_t *inst, uint32_t i2c)
{
    if (ak_i2c_ready(inst))
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

i2c_handle_t ak_i2c_init(I2C_ID id)
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

static amk_error_t i2c_write(i2c_handle_t i2c, uint8_t addr, uint8_t reg, uint8_t reg_size, const void* data, size_t length, size_t timeout)
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
                for(int i = 0; i < length+reg_size; i++) {
                    if(i2c_wait_flag_timeout(I2C0, I2C_FLAG_TBE, SET, timeout, start)) {
                        error = AMK_I2C_TIMEOUT;
                        i2c_debug("I2C GD32: i2c_write_reg: TBE timeout, index=%d, breakout\n", i);
                        break;
                    } 

                    // transmit register first
                    if (i == 0 && reg_size) {
                        i2c_data_transmit(I2C0, reg);
                    } else {
                        i2c_data_transmit(I2C0, p[i-reg_size]);
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

static amk_error_t i2c_write_dma(i2c_handle_t i2c, uint8_t addr, uint8_t reg, uint8_t reg_size, const void* data, size_t length, size_t timeout)
{
    uint32_t start = timer_read32();
    uint32_t state = I2C_START;
    bool quit = false;
    amk_error_t error = AMK_SUCCESS;
    i2c_instance_t *inst = (i2c_instance_t*)i2c;
    if (inst->busy) {
        if (dma_flag_get(DMA0, DMA_CH5, DMA_FLAG_FTF)) {
            dma_flag_clear(DMA0, DMA_CH5,DMA_FLAG_G);
            dma_channel_disable(DMA0, DMA_CH5);
            i2c_dma_config(I2C0, I2C_DMA_OFF);
            state = I2C_STOP;
            inst->busy = false;
        } else {
            return AMK_I2C_BUSY;
        }
    }

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
                if (length > 3) {
                    dma_memory_address_config(DMA0, DMA_CH5, (uint32_t)(data));
                    dma_transfer_number_config(DMA0, DMA_CH5, length);

                    i2c_dma_config(I2C0, I2C_DMA_ON);
                    dma_channel_enable(DMA0, DMA_CH5);
                    #if 0
                    while(!dma_flag_get(DMA0, DMA_CH5, DMA_FLAG_FTF));
                    dma_flag_clear(DMA0, DMA_CH5,DMA_FLAG_G);
                    dma_channel_disable(DMA0, DMA_CH5);
                    i2c_dma_config(I2C0, I2C_DMA_OFF);
                    #else
                    inst->busy = true;
                    return AMK_I2C_BUSY;
                    #endif
                } else {
                    uint8_t *p = (uint8_t *)data;
                    for(int i = 0; i < length+reg_size; i++) {
                        if(i2c_wait_flag_timeout(I2C0, I2C_FLAG_TBE, SET, timeout, start)) {
                            error = AMK_I2C_TIMEOUT;
                            i2c_debug("I2C GD32: i2c_write_reg: TBE timeout, index=%d, breakout\n", i);
                            break;
                        } 

                        // transmit register first
                        if (i == 0 && reg_size) {
                            i2c_data_transmit(I2C0, reg);
                        } else {
                            i2c_data_transmit(I2C0, p[i-reg_size]);
                        }
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

amk_error_t ak_i2c_send(i2c_handle_t i2c, uint8_t addr, const void* data, size_t length, size_t timeout)
{
    return i2c_write(i2c, addr, 0, 0, data, length, timeout);
}

amk_error_t ak_i2c_recv(i2c_handle_t i2c, uint8_t addr, void* data, size_t length, size_t timeout)
{
    return AMK_ERROR;
}

amk_error_t ak_i2c_write_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, const void* data, size_t length, size_t timeout)
{
    return i2c_write(i2c, addr, reg, 1, data, length, timeout);
}

amk_error_t ak_i2c_read_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, void* data, size_t length, size_t timeout)
{
    return AMK_SUCCESS;
}

void ak_i2c_uninit(i2c_handle_t i2c)
{
    //i2c_instance_t *inst = (i2c_instance_t*)i2c;
}

amk_error_t ak_i2c_send_async(i2c_handle_t i2c, uint8_t addr, const void *data, size_t length)
{
    return i2c_write_dma(i2c, addr, 0, 0, data, length, 10);
}
