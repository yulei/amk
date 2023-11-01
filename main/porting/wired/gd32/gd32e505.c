/**
 * @file gd32e505.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "amk_hal.h"
#include "cm_misc.h"
#include "wait.h"
#include "usb_common.h"
#include "amk_printf.h"

#include "tusb.h"

void system_clock_init(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    }while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)){
        while(1){
        }
    }

    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT(4);

    RCU_APB1EN |= RCU_APB1EN_PMUEN;

    /* HXTAL is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB/1 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB/2 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;

    /* CK_PLL = (CK_PREDIV0) * 42 = 168 MHz */ 
    RCU_CFG0 &= ~(RCU_CFG0_PLLMF | RCU_CFG0_PLLMF_4 | RCU_CFG0_PLLMF_5);
    RCU_CFG0 |= (RCU_PLLSRC_HXTAL_IRC48M | RCU_PLL_MUL42);

    /* CK_PREDIV0 = (CK_HXTAL)/4 *8 /8 = 4 MHz */ 
    RCU_CFG1 &= ~(RCU_CFG1_PLLPRESEL | RCU_CFG1_PREDV0SEL | RCU_CFG1_PLL1MF | RCU_CFG1_PREDV1 | RCU_CFG1_PREDV0);
    RCU_CFG1 |= (RCU_PLLPRESRC_HXTAL | RCU_PREDV0SRC_CKPLL1 | RCU_PLL1_MUL8 | RCU_PREDV1_DIV4 | RCU_PREDV0_DIV8);

    /* enable PLL1 */
    RCU_CTL |= RCU_CTL_PLL1EN;
    /* wait till PLL1 is ready */
    while((RCU_CTL & RCU_CTL_PLL1STB) == 0U){
    }

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)){
    }

    /* enable the high-drive to extend the clock frequency to 180 MHz */
    PMU_CTL0 |= PMU_CTL0_HDEN;
    while(0U == (PMU_CS0 & PMU_CS0_HDRF)){
    }

    /* select the high-drive mode */
    PMU_CTL0 |= PMU_CTL0_HDS;
    while(0U == (PMU_CS0 & PMU_CS0_HDSRF)){
    }

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLL;

    /* wait until PLL is selected as system clock */
    while(0U == (RCU_CFG0 & RCU_SCSS_PLL)){
    }

    SystemCoreClockUpdate();
}

#define GD32_GCCFG          0x50000038UL
#define GD32_GCCFG_PWRON    (0x1UL<<16)

static void usb_custom_init(void)
{
    rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV3_5);

    rcu_periph_clock_enable(RCU_USBHS);
    
    REG32(GD32_GCCFG) |= GD32_GCCFG_PWRON;

    busy_delay_ms(20);

    rcu_pllusbpresel_config(RCU_PLLUSBPRESRC_HXTAL);
    rcu_pllusbpredv_config(RCU_PLLUSBPREDVSRC_HXTAL_IRC48M, RCU_PLLUSBPREDV_DIV2);
    rcu_pllusb_config(RCU_PLLUSB_MUL60);

    RCU_ADDCTL |= RCU_ADDCTL_PLLUSBEN;

    while((RCU_ADDCTL & RCU_ADDCTL)==0) {

    };

    NVIC_SetPriority(USBHS_IRQn, 1U);
}

#ifdef USE_SPI0
uint32_t hspi0 = SPI0;
static void spi0_init(void)
{
    rcu_periph_clock_enable(RCU_SPI0);
    gpio_pin_remap_config(GPIO_SPI0_REMAP, ENABLE);
    /* SPI0 GPIO config: NSS/PA15, SCK/PB3, MOSI/PB5 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_5);
    /* SPI0 GPIO config: MISO/PB4 */
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_4);

    /* SPI parameter config */
    spi_parameter_struct spi_init_struct;
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;//SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_8;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init_gd32(SPI0, &spi_init_struct);
    spi_enable(SPI0);
}
#endif

#ifdef USE_SPI1
uint32_t hspi1 = SPI1;
static void spi1_init(void)
{
    rcu_periph_clock_enable(RCU_SPI1);
    /* SPI1 GPIO config: NSS/PB12, SCK/PB13, MOSI/PB15 */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15);
    /* SPI0 GPIO config: MISO/PB14 */
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_14);

    /* SPI parameter config */
    spi_parameter_struct spi_init_struct;
    spi_init_struct.trans_mode           = SPI_BIDIRECTIONAL_TRANSMIT;//SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = /*SPI_CK_PL_HIGH_PH_2EDGE;*/
                                        SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_16;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init_gd32(SPI1, &spi_init_struct);
    spi_enable(SPI1);
}
#endif

void custom_board_init(void)
{
    rcu_periph_clock_enable(RCU_PMU);

    usb_custom_init();

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_AF);

    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP,ENABLE);

    #ifdef USE_SPI0
    spi0_init();
    #endif

    #ifdef USE_SPI1
    spi1_init();
    #endif

    #ifdef DYNAMIC_CONFIGURATION
    rcu_periph_clock_enable(RCU_PMU);
    rcu_periph_clock_enable(RCU_BKPI);

    pmu_backup_write_enable();
    uint32_t magic = bkp_read_data(BKP_DATA_2);
    //if (magic == 0) {
    if (magic > 0) {
        usb_setting |= USB_MSC_BIT;
    } else {
        usb_setting = 0;
    }
    amk_printf("usb_setting: %ld\n", usb_setting);
    bkp_write_data(BKP_DATA_2, 0);
#endif
}

void custom_board_task(void)
{}

void USBHS_IRQHandler (void)
{
    tud_int_handler(0);
}