/**
 * @file m484.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "generic_hal.h"
#include "tusb.h"
#include "amk_printf.h"

void custom_board_init(void)
{
}

void USBD20_IRQHandler(void)
{
    tud_int_handler(0);
}
