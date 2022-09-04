/**
 * @file gd32e505.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "generic_hal.h"
#include "gd32_util.h"
#include "wait.h"

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

    usb_delay_ms(20);

    rcu_pllusbpresel_config(RCU_PLLUSBPRESRC_HXTAL);
    rcu_pllusbpredv_config(RCU_PLLUSBPREDVSRC_HXTAL_IRC48M, RCU_PLLUSBPREDV_DIV2);
    rcu_pllusb_config(RCU_PLLUSB_MUL60);

    RCU_ADDCTL |= RCU_ADDCTL_PLLUSBEN;

    while((RCU_ADDCTL & RCU_ADDCTL)==0) {

    };

    NVIC_SetPriority(USBHS_IRQn, 0U);
}

void custom_board_init(void)
{
    rcu_periph_clock_enable(RCU_PMU);

    usb_custom_init();

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_AF);

    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP,ENABLE);
}

void custom_board_task(void)
{}

void USBHS_IRQHandler (void)
{
    tud_int_handler(0);
}