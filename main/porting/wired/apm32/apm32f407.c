/**
 * @file apm32f407.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "amk_hal.h"
#include "cm_misc.h"
#include "wait.h"

#include "tusb.h"

void system_clock_init(void)
{
}

static void usb_custom_init(void)
{
    GPIO_Config_T gpioConfig;

    /* Configure USB OTG GPIO */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB);
        
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_14, GPIO_AF_OTG_HS_FS);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_15, GPIO_AF_OTG_HS_FS);
        
    /* USB DM, DP pin configuration */
    gpioConfig.mode = GPIO_MODE_AF;
    gpioConfig.speed = GPIO_SPEED_100MHz;
    gpioConfig.otype = GPIO_OTYPE_PP;
    gpioConfig.pupd = GPIO_PUPD_NOPULL;
    gpioConfig.pin = GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Config(GPIOB, &gpioConfig);
}

void custom_board_init(void)
{
    usb_custom_init();

    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB);
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOC);
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOD);
}

void custom_board_task(void)
{}

void OTG_HS1_IRQHandler(void)
{
    tud_int_handler(0);
}