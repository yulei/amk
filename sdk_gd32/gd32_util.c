/**
 * @file gd32_util.c
 * 
 */
#include "gd32e10x.h"
#include "gd32_util.h"

#ifndef SYSTICK_FREQ
#define SYSTICK_FREQ    1
#endif

volatile static uint32_t systick_counter = 0;

// always 1K HZ
void systick_init(void)
{
    SysTick_Config(SystemCoreClock / (1000/SYSTICK_FREQ));
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable((uint8_t)SysTick_IRQn, 3U, 0U);
    //NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void systick_delay(uint32_t ticks)
{
    //usb_delay_ms(ticks);
    //return;

    uint32_t tick_start = systick_get_tick();
    uint32_t wait = ticks;

    /* Add a freq to guarantee minimum wait */
    if (wait < 0xFFFFFFFFUL)
    {
        wait += SYSTICK_FREQ;
    }

    while((systick_get_tick() - tick_start) < wait) {
        ;
    }
}

void systick_inc_tick(void)
{
    systick_counter += SYSTICK_FREQ;
}

uint32_t systick_get_tick(void)
{
    return systick_counter;
}

void systick_suspend(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

 void systick_resume(void)
{
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

void usb_delay_ms(uint32_t ms)
{
    //systick_delay(ms);
    usb_delay_us(ms*1000);
}

// approximate us delay
void usb_delay_us(uint32_t us)
{
    __IO uint32_t ticks = us * (SystemCoreClock/1000000);
    while( ticks--) ;
}

//===================================
// utilities for the usb fs lib
//===================================

#include "drv_usb_hw.h"

//#define HOST_SOF_OUTPUT_RCC                     RCC_APB2PERIPH_GPIOA
//#define HOST_SOF_PORT                           GPIOA
//#define HOST_SOF_SIGNAL                         GPIO_PIN_8

/*!
    \brief      configure USB GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_gpio_config (void)
{
#ifdef USB_OTG_FS_LOW_PWR_MGMT_SUPPORT
    EXTI_InitPara EXTI_InitStructure;

    EXTI_ClearIntBitState(EXTI_LINE18);

    /* USB wakeup EXTI line configuration */
    EXTI_InitStructure.EXTI_LINE = EXTI_LINE18;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LINEEnable = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

#endif
}

/*!
    \brief      configure USB clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_rcu_config(void)
{
    static __IO uint32_t usbfs_prescaler = 0U;

    uint32_t system_clock = rcu_clock_freq_get(CK_SYS);
  
    if (48000000U == system_clock) {
        usbfs_prescaler = RCU_CKUSB_CKPLL_DIV1;
    } else if (72000000U == system_clock) {
        usbfs_prescaler = RCU_CKUSB_CKPLL_DIV1_5;
    } else if (96000000U == system_clock) {
        usbfs_prescaler = RCU_CKUSB_CKPLL_DIV2;
    } else if (120000000U == system_clock) {
        usbfs_prescaler = RCU_CKUSB_CKPLL_DIV2_5;
    }  else {
        /*  reserved  */
    }

#ifndef USE_IRC48M
    rcu_usb_clock_config(usbfs_prescaler);
#else
    /* enable IRC48M clock */
    rcu_osci_on(RCU_IRC48M);

    /* wait till IRC48M is ready */
    while (SUCCESS != rcu_osci_stab_wait(RCU_IRC48M)) {
    }

    rcu_ck48m_clock_config(RCU_CK48MSRC_IRC48M);
#endif /* USE_IRC48M */
    rcu_periph_clock_enable(RCU_USBFS);
}

/*!
    \brief      configure USB global interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_intr_config (void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable((uint8_t)USBFS_IRQn, 2U, 0U);

#if USB_LOW_POWER

    /* enable the power module clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* USB wakeup EXTI line configuration */
    exti_interrupt_flag_clear(EXTI_18);
    exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_enable(EXTI_18);

    nvic_irq_enable((uint8_t)USBFS_WKUP_IRQn, 1U, 0U);

#endif /* USB_LOW_POWER */
}

/*!
    \brief      drives the VBUS signal through gpio
    \param[in]  state: VBUS states
    \param[out] none
    \retval     none
*/
void usb_vbus_drive (uint8_t state)
{
    if (0U == state) {
        /* disable is needed on output of the power switch */
        //gpio_bit_set(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
    } else {
        /* enable the power switch by driving the enable low */
        //gpio_bit_reset(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
    }
}

/*!
    \brief      configures the GPIO for the VBUS
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_vbus_config (void)
{
    //rcu_periph_clock_enable(HOST_POWERSW_PORT_RCC);

    //gpio_init(HOST_POWERSW_PORT, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, HOST_POWERSW_VBUS);

    /* by default, disable is needed on output of the power switch */
    //usb_vbus_drive(0U);

    /* delay is need for stabilizing the VBUS low in reset condition,
     * when VBUS=1 and reset-button is pressed by user 
     */
    //usb_mdelay (200U);
}

/*!
    \brief      initializes delay unit using Timer2
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_timer_init (void)
{
    /* set the vector table base address at 0x08000000 */
    //nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x00U);

    /* configure the priority group to 2 bits */
    //nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

    /* enable the Timer2 global interrupt */
    //nvic_irq_enable((uint8_t)TIMER2_IRQn, 1U, 0U);

    //rcu_periph_clock_enable(RCU_TIMER2);
}

/*!
    \brief      delay in micro seconds
    \param[in]  usec: value of delay required in micro seconds
    \param[out] none
    \retval     none
*/
void usb_udelay (const uint32_t usec)
{
    usb_delay_us(usec);
}

/*!
    \brief      delay in milliseconds
    \param[in]  msec: value of delay required in milliseconds
    \param[out] none
    \retval     none
*/
void usb_mdelay (const uint32_t msec)
{
    usb_delay_ms(msec);
}

#ifdef USE_IRC48M
/*!
    \brief      configure the CTC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_config(void)
{
    /* configure CTC reference signal source prescaler */
    ctc_refsource_prescaler_config(CTC_REFSOURCE_PSC_OFF);
    /* select reference signal source */
    ctc_refsource_signal_select(CTC_REFSOURCE_USBSOF);
    /* select reference signal source polarity */
    ctc_refsource_polarity_config(CTC_REFSOURCE_POLARITY_RISING);
    /* configure hardware automatically trim mode */
    ctc_hardware_trim_mode_config(CTC_HARDWARE_TRIM_MODE_ENABLE);
    
    /* configure CTC counter reload value, Fclock/Fref-1 */
    ctc_counter_reload_value_config(0xBB7FU);
    /* configure clock trim base limit value, Fclock/Fref*0.0012/2 */
    ctc_clock_limit_value_config(0x1DU);

    /* CTC counter enable */
    ctc_counter_enable();
}
#endif /* USE_IRC48M */

