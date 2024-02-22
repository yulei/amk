/**
 * @file at32f405.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "amk_hal.h"
#include "cm_misc.h"
#include "wait.h"
#include "usb_common.h"

#include "tusb.h"

// if want to USE USB_HS, the HEXT MUST be 12000000

/**
  * @brief  system clock config program
  * @note   the system clock is configured as follow:
  *         system clock (sclk)   = (hext * pll_ns)/(pll_ms * pll_fp)
  *         system clock source   = pll (hext)
  *         - hext                = HEXT_VALUE
  *         - sclk                = 216000000
  *         - ahbdiv              = 1
  *         - ahbclk              = 216000000
  *         - apb2div             = 1
  *         - apb2clk             = 216000000
  *         - apb1div             = 2
  *         - apb1clk             = 108000000
  *         - pll_ns              = 72
  *         - pll_ms              = 1
  *         - pll_fr              = 4
  *         - flash_wtcyc         = 6 cycle
  * @param  none
  * @retval none
  */
void system_clock_init(void)
{
    /* reset crm */
    crm_reset();

    /* config flash psr register */
    flash_psr_set(FLASH_WAIT_CYCLE_6);

    /* enable pwc periph clock */
    crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);

    /* set power ldo output voltage to 1.3v */
    pwc_ldo_output_voltage_set(PWC_LDO_OUTPUT_1V3);

    crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);

    /* wait till hext is ready */
    while(crm_hext_stable_wait() == ERROR)
    {
    }

    /* if pll parameter has changed, please use the AT32_New_Clock_Configuration tool for new configuration. */
    crm_pll_config(CRM_PLL_SOURCE_HEXT, 72, 1, CRM_PLL_FP_4);

    /* config pllu div */
    crm_pllu_div_set(CRM_PLL_FU_18);

    /* enable pll */
    crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

    /* wait till pll is ready */
    while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET)
    {
    }

    /* config ahbclk */
    crm_ahb_div_set(CRM_AHB_DIV_1);

    /* config apb2clk, the maximum frequency of APB2 clock is 216 MHz */
    crm_apb2_div_set(CRM_APB2_DIV_1);

    /* config apb1clk, the maximum frequency of APB1 clock is 120 MHz */
    crm_apb1_div_set(CRM_APB1_DIV_2);

    /* enable auto step mode */
    crm_auto_step_mode_enable(TRUE);

    /* select pll as system clock source */
    crm_sysclk_switch(CRM_SCLK_PLL);

    /* wait till pll is used as system clock source */
    while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL)
    {
    }

    /* disable auto step mode */
    crm_auto_step_mode_enable(FALSE);

    /* update system_core_clock global variable */
    system_core_clock_update();
}


static void usb_custom_init(void)
{
    /* enable otg clock */
    crm_periph_clock_enable(CRM_OTGHS_PERIPH_CLOCK, TRUE);

    /* select usb 48m clcok source */
    //usb_clock48m_select(USB_CLK_HEXT);

    /* enable otg irq */
    nvic_irq_enable(OTGHS_IRQn, 0, 0);
}

#ifdef USE_SPI1
spi_type *hspi1 = SPI1;
static void spi1_gpio_init(void)
{
    gpio_init_type gpio_initstructure;
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

    /* master sck pin */
    gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
    gpio_initstructure.gpio_pull           = GPIO_PULL_DOWN;
    gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_initstructure.gpio_pins           = GPIO_PINS_5;
    gpio_init(GPIOA, &gpio_initstructure);
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE5, GPIO_MUX_5);

    /* master miso pin */
    gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
    gpio_initstructure.gpio_pins           = GPIO_PINS_6;
    gpio_init(GPIOA, &gpio_initstructure);
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE6, GPIO_MUX_5);

        /* master mosi pin */
    gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
    gpio_initstructure.gpio_pins           = GPIO_PINS_7;
    gpio_init(GPIOA, &gpio_initstructure);
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE7, GPIO_MUX_5);
}

static void spi1_init(void)
{
    spi1_gpio_init();

    crm_periph_clock_enable(CRM_SPI1_PERIPH_CLOCK, TRUE);
    spi_init_type spi_init_struct;
    spi_default_para_init(&spi_init_struct);
    spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
    spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
    spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_16;
    spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
    spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
    spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
    spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
    spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
    spi_init_at32(SPI1, &spi_init_struct);

    spi_enable(SPI1, TRUE);
}
#endif

#ifdef USE_I2C1
#include "i2c_application.h"
#define I2Cx_ADDRESS                     0x78 
//#define I2Cx_CLKCTRL                   0x4170FEFE   //10K
//#define I2Cx_CLKCTRL                   0x90F06666   //50K
#define I2Cx_CLKCTRL                     0x90F03030   //100K
//#define I2Cx_CLKCTRL                   0x20F07DDE   //200K
i2c_handle_type hi2c1;

static void i2c1_init(void)
{
    /* i2c peripheral initialization */
    gpio_init_type gpio_init_structure;


    /* configure i2c pins: scl */
    gpio_init_structure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_structure.gpio_mode           = GPIO_MODE_MUX;
    gpio_init_structure.gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN;
    gpio_init_structure.gpio_pull           = GPIO_PULL_UP;

    gpio_init_structure.gpio_pins           = GPIO_PINS_8;
    gpio_init(GPIOB, &gpio_init_structure);
    gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE8, GPIO_MUX_4);

    /* configure i2c pins: sda */
    gpio_init_structure.gpio_pins           = GPIO_PINS_9;
    gpio_init(GPIOB, &gpio_init_structure);
    gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE9, GPIO_MUX_4);

    /* config i2c */
    i2c_init_at32(hi2c1.i2cx, 0x0F, I2Cx_CLKCTRL);
    i2c_own_address1_set(hi2c1.i2cx, I2C_ADDRESS_MODE_7BIT, 0);

    /* i2c peripheral enable */
    i2c_enable(hi2c1.i2cx, TRUE);
    wait_ms(1);
}

static void i2c1_bus_reset(void)
{ 
    /* i2c periph clock enable */
    crm_periph_clock_enable(CRM_I2C1_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

    hi2c1.i2cx = I2C1;
    /* reset i2c peripheral */
    i2c_reset(hi2c1.i2cx);

    i2c1_init();
}
#endif
void custom_board_init(void)
{
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
    usb_custom_init();

    //usb_setting |= USB_MSC_BIT;

#ifdef USE_SPI1
    spi1_init();
#endif
#ifdef USE_I2C1
    i2c1_bus_reset();
#endif

#ifdef DYNAMIC_CONFIGURATION
    crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
    pwc_battery_powered_domain_access(TRUE);
    uint32_t magic = ertc_bpr_data_read(ERTC_DT2);

    //if (magic == 0) {
    if (magic > 0) {
        usb_setting |= USB_MSC_BIT;
    } else {
        usb_setting = 0;
    }
    ertc_bpr_data_write(ERTC_DT2, 0);
#endif
}

void custom_board_task(void)
{}

void OTGHS_IRQHandler(void)
{
    tud_int_handler(0);
}