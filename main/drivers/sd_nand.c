/**
 * @file sd_nand.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "sd_nand.h"

#if defined(STM32F405xx)
#   include "sd_nand_stm32.c"
#elif defined(STM32U575xx)
#   include "sd_nand_stm32u5.c"
#elif defined(STM32F722xx)
#   include "sd_nand_stm32f7.c"
#else
#   error "sd nand not available"
#endif