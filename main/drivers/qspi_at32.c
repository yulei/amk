/**
 * @file qspi_at32.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <string.h>

#include "qspi.h"
#include "amk_hal.h"
#include "amk_printf.h"

#ifndef QSPI_DEBUG
#define QSPI_DEBUG 1
#endif

#if QSPI_DEBUG
#define qspi_debug  amk_printf
#else
#define qspi_debug(...)
#endif

/* QSPCK Port/Pin definition */
#define QSPCK_PORT                      (GPIOB)
#define QSPCK_PIN                       (GPIO_PINS_4)
#define QSPCK_SRC                       (GPIO_PINS_SOURCE4)

/* QSNSS Port/Pin definition */
#define QSNSS_PORT                      (GPIOB)
#define QSNSS_PIN                       (GPIO_PINS_6)
#define QSNSS_SRC                       (GPIO_PINS_SOURCE6)

/* QSIO0 Port/Pin definition */
#define QSIO0_PORT                      (GPIOB)
#define QSIO0_PIN                       (GPIO_PINS_5)
#define QSIO0_SRC                       (GPIO_PINS_SOURCE5)

/* QSIO1 Port/Pin definition */
#define QSIO1_PORT                      (GPIOB)
#define QSIO1_PIN                       (GPIO_PINS_7)
#define QSIO1_SRC                       (GPIO_PINS_SOURCE7)

/* QSIO2 Port/Pin definition */
#define QSIO2_PORT                      (GPIOA)
#define QSIO2_PIN                       (GPIO_PINS_15)
#define QSIO2_SRC                       (GPIO_PINS_SOURCE15)

/* QSIO3 Port/Pin definition */
#define QSIO3_PORT                      (GPIOB)
#define QSIO3_PIN                       (GPIO_PINS_3)
#define QSIO3_SRC                       (GPIO_PINS_SOURCE3)

/* QSPI memory bus address definition */
#define QSPI_BUS_ADDRESS                (QSPI1_MEM_BASE)

/* FLASH parameters definition */
#define QSPI_FLASH_ADDR_SIZE            (23)
#define QSPI_FLASH_PAGE_SIZE            (0x100u)
#define QSPI_FLASH_SECTOR_SIZE          (0x1000u)
#define QSPI_FLASH_MAX_ADDR             (0x1000000u)
#define QSPI_FLASH_DUMMY_BYTE_VALUE     (0xffu)
#define QSPI_FLASH_BUSY_BIT_MASK        (0x01u)

/* FLASH instruction definition */
#define FLASH_INSTR_WRITE_ENABLE        (0x06u)
#define FLASH_INSTR_PAGE_PROGRAM        (0x02u)
#define FLASH_INSTR_PAGE_PROGRAM_QUAD   (0x32U)
#define FLASH_INSTR_ERASE_4KB_SECTOR    (0x20u)
#define FLASH_INSTR_ERASE_CHIP          (0xC7u)
#define FLASH_INSTR_READ_SR1            (0x05u)
#define FLASH_INSTR_READ_SR2            (0x35u)
#define FLASH_INSTR_READ_SR3            (0x15u)
#define FLASH_INSTR_WRITE_SR1           (0x01u)
#define FLASH_INSTR_WRITE_SR2           (0x31u)
#define FLASH_INSTR_WRITE_SR3           (0x11u)
#define FLASH_INSTR_READ_CMD            (0x03)
#define FLASH_INSTR_QUAD_READ_CMD       (0x6B)
#define FLASH_INSTR_QUAD_READ_IO_CMD    (0xEB)
#define FLASH_INSTR_RESET_ENABLE        (0x66U)
#define FLASH_INSTR_RESET               (0x99U)
#define FLASH_INSTR_JEDECID             (0x9FU)

/* TIMEOUT*/
#define FLASH_DUMMY_CYCLES_READ_QUAD    (10)
#define FLASH_BULK_ERASE_MAX_TIME       (250000)
#define FLASH_SECTOR_ERASE_MAX_TIME     (3000)
#define FLASH_SUBSECTOR_ERASE_MAX_TIME  (800)

/* STATUS */
#define W25Q128FV_FSR_BUSY                    ((uint8_t)0x01)    /*!< busy */
#define W25Q128FV_FSR_WREN                    ((uint8_t)0x02)    /*!< write enable */
#define W25Q128FV_FSR_QE                      ((uint8_t)0x02)    /*!< quad enable */

static qspi_cmd_type w25qxx_cmd_config;
static qspi_xip_type w25qxx_xip_init;

static void qspi_busy_check(void);
static void qspi_write_enable(void);

static void qspi_cmd_read_config(qspi_cmd_type *qspi_cmd_struct, uint32_t addr, uint32_t counter)
{
  qspi_cmd_struct->pe_mode_enable = FALSE;
  qspi_cmd_struct->pe_mode_operate_code = 0;
  qspi_cmd_struct->instruction_code = FLASH_INSTR_QUAD_READ_IO_CMD;
  qspi_cmd_struct->instruction_length = QSPI_CMD_INSLEN_1_BYTE;
  qspi_cmd_struct->address_code = addr;
  qspi_cmd_struct->address_length = QSPI_CMD_ADRLEN_3_BYTE;
  qspi_cmd_struct->data_counter = counter;
  qspi_cmd_struct->second_dummy_cycle_num = 6;
  qspi_cmd_struct->operation_mode = QSPI_OPERATE_MODE_144;
  qspi_cmd_struct->read_status_config = QSPI_RSTSC_HW_AUTO;
  qspi_cmd_struct->read_status_enable = FALSE;
  qspi_cmd_struct->write_data_enable = FALSE;
}

// write command
static void qspi_cmd_write_config(qspi_cmd_type *qspi_cmd_struct, uint32_t addr, uint32_t counter)
{
    qspi_cmd_struct->pe_mode_enable = FALSE;
    qspi_cmd_struct->pe_mode_operate_code = 0;
    qspi_cmd_struct->instruction_code = FLASH_INSTR_PAGE_PROGRAM_QUAD;
    qspi_cmd_struct->instruction_length = QSPI_CMD_INSLEN_1_BYTE;
    qspi_cmd_struct->address_code = addr;
    qspi_cmd_struct->address_length = QSPI_CMD_ADRLEN_3_BYTE;
    qspi_cmd_struct->data_counter = counter;
    qspi_cmd_struct->second_dummy_cycle_num = 0;
    qspi_cmd_struct->operation_mode = QSPI_OPERATE_MODE_114;
    qspi_cmd_struct->read_status_config = QSPI_RSTSC_HW_AUTO;
    qspi_cmd_struct->read_status_enable = FALSE;
    qspi_cmd_struct->write_data_enable = TRUE;
}

// erase command
static void qspi_cmd_erase_config(qspi_cmd_type *qspi_cmd_struct, uint32_t addr)
{
    qspi_cmd_struct->pe_mode_enable = FALSE;
    qspi_cmd_struct->pe_mode_operate_code = 0;
    qspi_cmd_struct->instruction_code = FLASH_INSTR_ERASE_4KB_SECTOR;
    qspi_cmd_struct->instruction_length = QSPI_CMD_INSLEN_1_BYTE;
    qspi_cmd_struct->address_code = addr;
    qspi_cmd_struct->address_length = QSPI_CMD_ADRLEN_3_BYTE;
    qspi_cmd_struct->data_counter = 0;
    qspi_cmd_struct->second_dummy_cycle_num = 0;
    qspi_cmd_struct->operation_mode = QSPI_OPERATE_MODE_111;
    qspi_cmd_struct->read_status_config = QSPI_RSTSC_HW_AUTO;
    qspi_cmd_struct->read_status_enable = FALSE;
    qspi_cmd_struct->write_data_enable = TRUE;
}

// write enable command
static void qspi_cmd_wren_config(qspi_cmd_type *qspi_cmd_struct)
{
    qspi_cmd_struct->pe_mode_enable = FALSE;
    qspi_cmd_struct->pe_mode_operate_code = 0;
    qspi_cmd_struct->instruction_code = FLASH_INSTR_WRITE_ENABLE;
    qspi_cmd_struct->instruction_length = QSPI_CMD_INSLEN_1_BYTE;
    qspi_cmd_struct->address_code = 0;
    qspi_cmd_struct->address_length = QSPI_CMD_ADRLEN_0_BYTE;
    qspi_cmd_struct->data_counter = 0;
    qspi_cmd_struct->second_dummy_cycle_num = 0;
    qspi_cmd_struct->operation_mode = QSPI_OPERATE_MODE_111;
    qspi_cmd_struct->read_status_config = QSPI_RSTSC_HW_AUTO;
    qspi_cmd_struct->read_status_enable = FALSE;
    qspi_cmd_struct->write_data_enable = TRUE;
}

// read SR1 command
static void qspi_cmd_rdsr_config(qspi_cmd_type *qspi_cmd_struct)
{
    qspi_cmd_struct->pe_mode_enable = FALSE;
    qspi_cmd_struct->pe_mode_operate_code = 0;
    qspi_cmd_struct->instruction_code = FLASH_INSTR_READ_SR1;
    qspi_cmd_struct->instruction_length = QSPI_CMD_INSLEN_1_BYTE;
    qspi_cmd_struct->address_code = 0;
    qspi_cmd_struct->address_length = QSPI_CMD_ADRLEN_0_BYTE;
    qspi_cmd_struct->data_counter = 0;
    qspi_cmd_struct->second_dummy_cycle_num = 0;
    qspi_cmd_struct->operation_mode = QSPI_OPERATE_MODE_111;
    qspi_cmd_struct->read_status_config = QSPI_RSTSC_HW_AUTO;
    qspi_cmd_struct->read_status_enable = TRUE;
    qspi_cmd_struct->write_data_enable = FALSE;
}

// reset enable command
static void qspi_cmd_rsten_config(qspi_cmd_type *qspi_cmd_struct)
{
    qspi_cmd_struct->pe_mode_enable = FALSE;
    qspi_cmd_struct->pe_mode_operate_code = 0;
    qspi_cmd_struct->instruction_code = FLASH_INSTR_RESET_ENABLE;
    qspi_cmd_struct->instruction_length = QSPI_CMD_INSLEN_1_BYTE;
    qspi_cmd_struct->address_code = 0;
    qspi_cmd_struct->address_length = QSPI_CMD_ADRLEN_0_BYTE;
    qspi_cmd_struct->data_counter = 0;
    qspi_cmd_struct->second_dummy_cycle_num = 0;
    qspi_cmd_struct->operation_mode = QSPI_OPERATE_MODE_111;
    qspi_cmd_struct->read_status_config = QSPI_RSTSC_HW_AUTO;
    qspi_cmd_struct->read_status_enable = FALSE;
    qspi_cmd_struct->write_data_enable = TRUE;
}

// reset command
static void qspi_cmd_rst_config(qspi_cmd_type *qspi_cmd_struct)
{
    qspi_cmd_struct->pe_mode_enable = FALSE;
    qspi_cmd_struct->pe_mode_operate_code = 0;
    qspi_cmd_struct->instruction_code = FLASH_INSTR_RESET;
    qspi_cmd_struct->instruction_length = QSPI_CMD_INSLEN_1_BYTE;
    qspi_cmd_struct->address_code = 0;
    qspi_cmd_struct->address_length = QSPI_CMD_ADRLEN_0_BYTE;
    qspi_cmd_struct->data_counter = 0;
    qspi_cmd_struct->second_dummy_cycle_num = 0;
    qspi_cmd_struct->operation_mode = QSPI_OPERATE_MODE_111;
    qspi_cmd_struct->read_status_config = QSPI_RSTSC_HW_AUTO;
    qspi_cmd_struct->read_status_enable = FALSE;
    qspi_cmd_struct->write_data_enable = TRUE;
}

// erase chip command
static void qspi_cmd_erase_chip_config(qspi_cmd_type *qspi_cmd_struct)
{
    qspi_cmd_struct->pe_mode_enable = FALSE;
    qspi_cmd_struct->pe_mode_operate_code = 0;
    qspi_cmd_struct->instruction_code = FLASH_INSTR_ERASE_CHIP;
    qspi_cmd_struct->instruction_length = QSPI_CMD_INSLEN_1_BYTE;
    qspi_cmd_struct->address_code = 0;
    qspi_cmd_struct->address_length = QSPI_CMD_ADRLEN_0_BYTE;
    qspi_cmd_struct->data_counter = 0;
    qspi_cmd_struct->second_dummy_cycle_num = 0;
    qspi_cmd_struct->operation_mode = QSPI_OPERATE_MODE_111;
    qspi_cmd_struct->read_status_config = QSPI_RSTSC_HW_AUTO;
    qspi_cmd_struct->read_status_enable = FALSE;
    qspi_cmd_struct->write_data_enable = TRUE;
}

static void qspi_cmd_send(qspi_cmd_type* cmd)
{
  qspi_cmd_operation_kick(QSPI1, cmd);
  /* wait command completed */
  while(qspi_flag_get(QSPI1, QSPI_CMDSTS_FLAG) == RESET);
  qspi_flag_clear(QSPI1, QSPI_CMDSTS_FLAG);
}

static void qspi_busy_check(void)
{
    qspi_cmd_rdsr_config(&w25qxx_cmd_config);
    qspi_cmd_operation_kick(QSPI1, &w25qxx_cmd_config);

    /* wait command completed */
    while(qspi_flag_get(QSPI1, QSPI_CMDSTS_FLAG) == RESET);
    qspi_flag_clear(QSPI1, QSPI_CMDSTS_FLAG);
}

static void qspi_write_enable(void)
{
    qspi_cmd_wren_config(&w25qxx_cmd_config);
    qspi_cmd_operation_kick(QSPI1, &w25qxx_cmd_config);

    /* wait command completed */
    while(qspi_flag_get(QSPI1, QSPI_CMDSTS_FLAG) == RESET);
    qspi_flag_clear(QSPI1, QSPI_CMDSTS_FLAG);
}

static void qspi_xip_init_config(qspi_xip_type *qspi_xip_struct)
{
    qspi_xip_struct->read_instruction_code = FLASH_INSTR_QUAD_READ_CMD; 
    qspi_xip_struct->read_address_length = QSPI_XIP_ADDRLEN_3_BYTE;
    qspi_xip_struct->read_operation_mode = QSPI_OPERATE_MODE_114;
    qspi_xip_struct->read_second_dummy_cycle_num = 8;
    qspi_xip_struct->write_instruction_code = FLASH_INSTR_PAGE_PROGRAM_QUAD;
    qspi_xip_struct->write_address_length = QSPI_XIP_ADDRLEN_3_BYTE;
    qspi_xip_struct->write_operation_mode = QSPI_OPERATE_MODE_114;
    qspi_xip_struct->write_second_dummy_cycle_num = 0;
    qspi_xip_struct->write_select_mode = QSPI_XIPW_SEL_MODED;
    qspi_xip_struct->write_time_counter = 0x7F;
    qspi_xip_struct->write_data_counter = 0x1F;
    qspi_xip_struct->read_select_mode = QSPI_XIPR_SEL_MODED;
    qspi_xip_struct->read_time_counter = 0x7F;
    qspi_xip_struct->read_data_counter = 0x1F;
}

static void qspi_xip_switch(bool enable)
{
    if (enable) {
        /* switch to command-port mode */
        qspi_xip_enable(QSPI1, FALSE);

        /* issue reset command */
        qspi_cmd_rsten_config(&w25qxx_cmd_config);
        qspi_cmd_send(&w25qxx_cmd_config);
        qspi_cmd_rst_config(&w25qxx_cmd_config);
        qspi_cmd_send(&w25qxx_cmd_config);
        
        /* initial xip */
        qspi_xip_init_config(&w25qxx_xip_init);
        qspi_xip_init(QSPI1, &w25qxx_xip_init);
        qspi_xip_enable(QSPI1, TRUE);
    } else {
        qspi_xip_enable(QSPI1, FALSE);
    }
}

bool qspi_init(uint32_t map_addr)
{
    gpio_init_type gpio_init_struct;

    /* enable the qspi clock */
    crm_periph_clock_enable(CRM_QSPI1_PERIPH_CLOCK, TRUE);

    /* enable the pin clock */
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

    /* set default parameter */
    gpio_default_para_init(&gpio_init_struct);

    /* configure the io0 gpio */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

    gpio_init_struct.gpio_pins = QSIO0_PIN;
    gpio_init(QSIO0_PORT, &gpio_init_struct);
    gpio_pin_mux_config(QSIO0_PORT, QSIO0_SRC, GPIO_MUX_11);

    /* configure the io1 gpio */
    gpio_init_struct.gpio_pins = QSIO1_PIN;
    gpio_init(QSIO1_PORT, &gpio_init_struct);
    gpio_pin_mux_config(QSIO1_PORT, QSIO1_SRC, GPIO_MUX_11);

    /* configure the io2 gpio */
    gpio_init_struct.gpio_pins = QSIO2_PIN;
    gpio_init(QSIO2_PORT, &gpio_init_struct);
    gpio_pin_mux_config(QSIO2_PORT, QSIO2_SRC, GPIO_MUX_11);

    /* configure the io3 gpio */
    gpio_init_struct.gpio_pins = QSIO3_PIN;
    gpio_init(QSIO3_PORT, &gpio_init_struct);
    gpio_pin_mux_config(QSIO3_PORT, QSIO3_SRC, GPIO_MUX_11);

    /* configure the sck gpio */
    gpio_init_struct.gpio_pins = QSPCK_PIN;
    gpio_init(QSPCK_PORT, &gpio_init_struct);
    gpio_pin_mux_config(QSPCK_PORT, QSPCK_SRC, GPIO_MUX_11);

    /* configure the cs gpio */
    gpio_init_struct.gpio_pins = QSNSS_PIN;
    gpio_init(QSNSS_PORT, &gpio_init_struct);
    gpio_pin_mux_config(QSNSS_PORT, QSNSS_SRC, GPIO_MUX_11);

    /* switch to cmd port */
    qspi_xip_switch(FALSE);

    /* set sclk */
    qspi_clk_division_set(QSPI1, QSPI_CLK_DIV_4);

    /* set sck idle mode 0 */
    qspi_sck_mode_set(QSPI1, QSPI_SCK_MODE_0);

    /* set wip in bit 0 */
    qspi_busy_config(QSPI1, QSPI_BUSY_OFFSET_0);

    /* switch to xip mode*/
    //qspi_xip_switch(TRUE);

    return true;
}

#define QSPI_FIFO_DEPTH                  (32*4)

amk_error_t qspi_read_sector(uint32_t address, uint8_t *buffer, size_t length)
{
    if (length != QSPI_FLASH_SECTOR_SIZE) {
        qspi_debug("QSPI: read_sector: invalid size:%d\n", length);
        return AMK_QSPI_INVALID_PARAM;
    }

#if 0
    uint8_t *src = (uint8_t *)(QSPI_BUS_ADDRESS + address);
    qspi_debug("QSPI: read_sector: addr=%p, size=%d\n", src, length);
    memcpy(buffer, src, length);
#else
    uint32_t i, len = length;

    qspi_cmd_read_config(&w25qxx_cmd_config, address, length);
    qspi_cmd_operation_kick(QSPI1, &w25qxx_cmd_config);

    /* read data */
    do
    {
        if(length >= QSPI_FIFO_DEPTH) {
            len = QSPI_FIFO_DEPTH;
        } else {
            len = length;
        }

        while(qspi_flag_get(QSPI1, QSPI_RXFIFORDY_FLAG) == RESET) {
        }

        for(i = 0; i < len; ++i) {
            *buffer ++ = qspi_byte_read(QSPI1);
        }
        length -= len;
    }while(length);

    /* wait command completed */
    while(qspi_flag_get(QSPI1, QSPI_CMDSTS_FLAG) == RESET) {
    }  
    qspi_flag_clear(QSPI1, QSPI_CMDSTS_FLAG);
#endif
    return AMK_SUCCESS;
}

// erase sector
static void qspi_erase(uint32_t sec_addr)
{
  qspi_write_enable();

  qspi_cmd_erase_config(&w25qxx_cmd_config, sec_addr);
  qspi_cmd_operation_kick(QSPI1, &w25qxx_cmd_config);

  /* wait command completed */
  while(qspi_flag_get(QSPI1, QSPI_CMDSTS_FLAG) == RESET);
  qspi_flag_clear(QSPI1, QSPI_CMDSTS_FLAG);

  qspi_busy_check();
}

amk_error_t qspi_write_sector(uint32_t address, const uint8_t* buffer, size_t length)
{
    //qspi_debug("QSPI WRITE: addr=%d, size=%d\n", address, length);
    if (length != QSPI_FLASH_SECTOR_SIZE) {
        qspi_debug("QSPI: write_sector: invalid size:%u\n", length);
        return AMK_QSPI_INVALID_PARAM;
    }

    // erase sector first
    if (address >= QSPI_FLASH_MAX_ADDR) {
        qspi_debug("QSPI: write_sector: invalid address:%u\n", address);
        return AMK_QSPI_INVALID_PARAM;
    } else {
        //qspi_xip_switch(FALSE);
        qspi_erase(address);
    }

    // then program
    uint32_t addr = address;
    const uint8_t *cur = buffer;
    for (int i = 0; i < length/QSPI_FLASH_PAGE_SIZE; i++) {
        qspi_write_enable();
        /* send up to 256 bytes at one time, and only one page */
        qspi_cmd_write_config(&w25qxx_cmd_config, addr, QSPI_FLASH_PAGE_SIZE);
        qspi_cmd_operation_kick(QSPI1, &w25qxx_cmd_config);

        uint32_t len = QSPI_FLASH_PAGE_SIZE;
        while (len--) {
            while(qspi_flag_get(QSPI1, QSPI_TXFIFORDY_FLAG) == RESET);
            qspi_byte_write(QSPI1, *cur++);
        }

        /* wait command completed */
        while(qspi_flag_get(QSPI1, QSPI_CMDSTS_FLAG) == RESET);
        qspi_flag_clear(QSPI1, QSPI_CMDSTS_FLAG);

        qspi_busy_check();

        addr += QSPI_FLASH_PAGE_SIZE;
    }

    //qspi_xip_switch(TRUE);
    return AMK_SUCCESS;
}

amk_error_t qspi_erase_chip(void)
{
    //qspi_xip_switch(FALSE);
    qspi_write_enable();
    qspi_cmd_erase_chip_config(&w25qxx_cmd_config);
    qspi_cmd_operation_kick(QSPI1, &w25qxx_cmd_config);

    while(qspi_flag_get(QSPI1, QSPI_CMDSTS_FLAG) == RESET);
    qspi_flag_clear(QSPI1, QSPI_CMDSTS_FLAG);

    qspi_busy_check();
    //qspi_xip_switch(TRUE);

    return AMK_SUCCESS;
}

void qspi_uninit(void)
{
}
