/**
 * @file gd32.c
 */

#include "amk_gpio.h"
#include "usb_descriptors.h"
#include "usb_comp.h"
#include "drv_usb_hw.h"
#include "drv_usbd_int.h"

volatile static uint32_t systick_count;
extern usb_core_driver amk_comp;

extern void usb_timer_irq(void);

static void systick_init(void) 
{
    if (SysTick_Config(SystemCoreClock / 1000U)){
        while (1){
        }
    }

    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable((uint8_t)SysTick_IRQn, 2U, 0U);
}

static void systick_inc(void)
{
    systick_count++;
}

void system_init(void)
{
    systick_init();

	//usb_rcu_config();
    //usb_timer_init();
    //usbd_init(&amk_comp, USB_CORE_ENUM_FS, &comp_desc, &usbd_comp_cb);
    //usb_dev_disconnect(&msc_udisk);
    //usb_intr_config();
}

void custom_board_init(void)
{
}

void custom_board_task(void)
{
}

uint32_t systick_get_tick(void)
{
    return systick_count;
}

void sdk_init()
{
    systick_init();
}

uint32_t sdk_ticks(void)
{
    return systick_get_tick();
}

// system irq handler
void SysTick_Handler(void)
{
    systick_inc();
}

void TIMER2_IRQHandler(void)
{
    usb_timer_irq();
}

void  USBFS_IRQHandler (void)
{
    usbd_isr(&amk_comp);
}
