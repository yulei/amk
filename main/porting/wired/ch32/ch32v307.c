/**
 * @file gd32e505.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "generic_hal.h"
#include "cm_misc.h"
#include "wait.h"

#include "tusb.h"

void USBHS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void USBHS_IRQHandler(void)
{
    tud_int_handler(0);
}

void custom_board_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
}

void custom_board_task(void)
{}
