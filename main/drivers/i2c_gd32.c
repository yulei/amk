 /**
  * i2c_stm32.c
  */

#include <stdbool.h>
#include <string.h>

#include "generic_hal.h"
#include "timer.h"
#include "i2c.h"
#include "amk_gpio.h"
#include "amk_printf.h"

typedef struct {
    uint32_t handle;
    bool ready;
} i2c_instance_t;

#ifdef USE_I2C1
    #define I2C1_ID     I2C0 
    #define I2C1_SCL    I2C1_SCL_PIN
    #define I2C1_SDA    I2C1_SDA_PIN
    static i2c_instance_t m_i2c1 = {
        .ready = false,
    };
    static void i2c1_config(void)
    {
        rcu_periph_clock_enable(RCU_GPIOB);
        rcu_periph_clock_enable(RCU_I2C0);
        rcu_periph_clock_enable(RCU_AF);

        i2c_deinit(I2C0);

        gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);

        i2c_clock_config(I2C0, 400000, I2C_DTCY_2);
        /* I2C address configure */
        i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x70);
        /* enable I2C0 */
        i2c_enable(I2C0);
        /* enable acknowledge */
        i2c_ack_config(I2C0, I2C_ACK_ENABLE);

    }
#endif

#ifdef USE_I2C2 
    #define I2C2_ID     I2C1
    #define I2C2_SCL    I2C2_SCL_PIN
    #define I2C2_SDA    I2C2_SDA_PIN
    static i2c_instance_t m_i2c2 = {
        .ready = false,
    };
#endif

static void i2c_bus_reset(void)
{
    /* configure SDA/SCL for GPIO */
    GPIO_BC(GPIOB) |= GPIO_PIN_6 | GPIO_PIN_7;
    gpio_init(GPIOB,GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    /* stop signal */
    GPIO_BOP(GPIOB) |= GPIO_PIN_6;
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    GPIO_BOP(GPIOB) |= GPIO_PIN_7;

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
        i2c1_config();
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

amk_error_t i2c_send(i2c_handle_t i2c, uint8_t addr, const void* data, size_t length, size_t timeout)
{
    //i2c_instance_t *inst = (i2c_instance_t*)i2c;
    //i2c_software_reset_config(I2C0, I2C_SRESET_SET);
    //i2c_software_reset_config(I2C0, I2C_SRESET_RESET);
    uint32_t start = timer_read32();

    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)) {
        if (timer_elapsed32(start) > timeout) {
            i2c_bus_reset();
            start = timer_read32();
            //break;
            //return AMK_I2C_TIMEOUT;
        }
    }

    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, addr, I2C_TRANSMITTER);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }

    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    /* wait until the transmit data buffer is empty */
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }

    const uint8_t *p= (const uint8_t*)data;
    for(int i=0; i<length; i++){
        /* data transmission */
        i2c_data_transmit(I2C0, p[i]);
        /* wait until the TBE bit is set */
        while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)) {
            if (timer_elapsed32(start) > timeout) {
                return AMK_I2C_TIMEOUT;
            }
        }
    }
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C0);
    while(I2C_CTL0(I2C0)&0x0200) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }

    return AMK_SUCCESS;
}

amk_error_t i2c_recv(i2c_handle_t i2c, uint8_t addr, void* data, size_t length, size_t timeout)
{
    //i2c_instance_t *inst = (i2c_instance_t*)i2c;
    //i2c_software_reset_config(I2C0, I2C_SRESET_SET);
    //i2c_software_reset_config(I2C0, I2C_SRESET_RESET);

    uint32_t start = timer_read32();
    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)) {
        if (timer_elapsed32(start) > timeout) {
            i2c_bus_reset();
            start = timer_read32();
            //return AMK_I2C_TIMEOUT;
        }
    }

    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }

    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, addr, I2C_RECEIVER);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    uint8_t *p = (uint8_t*)data;

    if (length == 1) {
        /* N=1,send stop condition after clearing ADDRSEND bit */
        i2c_stop_on_bus(I2C0);
        /* wait until the RBNE bit is set */
        while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
        /* read a data from I2C_DATA */
        p[0] = (uint8_t)i2c_data_receive(I2C0);

    } else if (length == 2) {
        while(!i2c_flag_get(I2C0, I2C_FLAG_BTC)){
            if (timer_elapsed32(start) > timeout) {
                return AMK_I2C_TIMEOUT;
            }
        }
        for (int i = 0; i < 2; i++) {
            /* wait until the RBNE bit is set */
            while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE)) {
                if (timer_elapsed32(start) > timeout) {
                    return AMK_I2C_TIMEOUT;
                }
            }

            /* read a data from I2C_DATA */
            p[i] = (uint8_t)i2c_data_receive(I2C0);
        }
        i2c_stop_on_bus(I2C0);
    } else {
        for(int i = 0; i < length; i++){
            if((length-3) == i){
                /* wait until the second last data byte is received into the shift register */
                while(!i2c_flag_get(I2C0, I2C_FLAG_BTC)) {
                    if (timer_elapsed32(start) > timeout) {
                        return AMK_I2C_TIMEOUT;
                    }
                }
                /* disable acknowledge */
                i2c_ack_config(I2C0, I2C_ACK_DISABLE);
            }

            /* wait until the RBNE bit is set */
            while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE)) {
                if (timer_elapsed32(start) > timeout) {
                    return AMK_I2C_TIMEOUT;
                }
            }
            /* read a data from I2C_DATA */
            p[i] = (uint8_t)i2c_data_receive(I2C0);
        }
        i2c_stop_on_bus(I2C0);
    }

    /* send a stop condition to I2C bus */
    while(I2C_CTL0(I2C0)&0x0200) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }
    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);

    return AMK_ERROR;
}

amk_error_t i2c_write_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, const void* data, size_t length, size_t timeout)
{
    //i2c_instance_t *inst = (i2c_instance_t*)i2c;
    //i2c_software_reset_config(I2C0, I2C_SRESET_SET);
    //i2c_software_reset_config(I2C0, I2C_SRESET_RESET);

    uint32_t start = timer_read32();

    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)) {
        if (timer_elapsed32(start) > timeout) {
            i2c_bus_reset();
            start = timer_read32();
            //return AMK_I2C_TIMEOUT;
        }
    }

    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND)){
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }

    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, addr, I2C_TRANSMITTER);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }

    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    /* wait until the transmit data buffer is empty */
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }

    /* write register address */
    i2c_data_transmit(I2C0, reg);
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }

    const uint8_t *p= (const uint8_t*)data;
    for(int i=0; i<length; i++){
        /* data transmission */
        i2c_data_transmit(I2C0, p[i]);
        /* wait until the TBE bit is set */
        while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)) {
            if (timer_elapsed32(start) > timeout) {
                return AMK_I2C_TIMEOUT;
            }
        }
    }
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C0);
    while(I2C_CTL0(I2C0)&0x0200) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }

    return AMK_SUCCESS;
}

amk_error_t i2c_read_reg(i2c_handle_t i2c, uint8_t addr, uint8_t reg, void* data, size_t length, size_t timeout)
{
    //i2c_instance_t *inst = (i2c_instance_t*)i2c;
    //i2c_software_reset_config(I2C0, I2C_SRESET_SET);
    //i2c_software_reset_config(I2C0, I2C_SRESET_RESET);

    uint32_t start = timer_read32();
    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)) {
        if (timer_elapsed32(start) > timeout) {
            i2c_bus_reset();
            start = timer_read32();
            //return AMK_I2C_TIMEOUT;
        }
    }

    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }

    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, addr, I2C_TRANSMITTER);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

    /* send register address */
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }
    i2c_data_transmit(I2C0, addr);
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }
    /* send a re-start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }

    /* re-send slave address to I2C bus */
    i2c_master_addressing(I2C0, addr, I2C_RECEIVER);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

    uint8_t *p = (uint8_t*)data;

    if (length == 1) {
        /* N=1,send stop condition after clearing ADDRSEND bit */
        i2c_stop_on_bus(I2C0);
        /* wait until the RBNE bit is set */
        while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
        /* read a data from I2C_DATA */
        p[0] = (uint8_t)i2c_data_receive(I2C0);

    } else if (length == 2) {
        while(!i2c_flag_get(I2C0, I2C_FLAG_BTC)){
            if (timer_elapsed32(start) > timeout) {
                return AMK_I2C_TIMEOUT;
            }
        }
        for (int i = 0; i < 2; i++) {
            /* wait until the RBNE bit is set */
            while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE)) {
                if (timer_elapsed32(start) > timeout) {
                    return AMK_I2C_TIMEOUT;
                }
            }

            /* read a data from I2C_DATA */
            p[i] = (uint8_t)i2c_data_receive(I2C0);
        }
        i2c_stop_on_bus(I2C0);
    } else {
        for(int i = 0; i < length; i++){
            if((length-3) == i){
                /* wait until the second last data byte is received into the shift register */
                while(!i2c_flag_get(I2C0, I2C_FLAG_BTC)) {
                    if (timer_elapsed32(start) > timeout) {
                        return AMK_I2C_TIMEOUT;
                    }
                }
                /* disable acknowledge */
                i2c_ack_config(I2C0, I2C_ACK_DISABLE);
            }

            /* wait until the RBNE bit is set */
            while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE)) {
                if (timer_elapsed32(start) > timeout) {
                    return AMK_I2C_TIMEOUT;
                }
            }
            /* read a data from I2C_DATA */
            p[i] = (uint8_t)i2c_data_receive(I2C0);
        }
        i2c_stop_on_bus(I2C0);
    }

    /* send a stop condition to I2C bus */
    while(I2C_CTL0(I2C0)&0x0200) {
        if (timer_elapsed32(start) > timeout) {
            return AMK_I2C_TIMEOUT;
        }
    }
    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);

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
