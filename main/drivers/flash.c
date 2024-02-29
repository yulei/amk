/**
 * @file flash.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#if defined(NRF52832_XXAA) || defined(NRF52840_XXAA)
#   include "flash_nrf52.c"
#elif defined(STM32F411xE) || defined(STM32F405xx) || defined(STM32F722xx) || defined(STM32L072xx) || defined(STM32F401xC) || defined(STM32F446xx) || defined(STM32G431xx) || defined(STM32L432xx) || defined(STM32F412Rx)
#   include "flash_stm32.c"
#elif defined(HC32F460xE)
#   include "flash_hc32.c"
#elif defined(GD32E505)
#   include "flash_gd32.c"
#elif defined(CH32V307)
#   include "flash_ch32.c"
#elif defined(AT32F405)
#   include "flash_at32.c"
#else
#   error "flash read/write not implemented"
#endif