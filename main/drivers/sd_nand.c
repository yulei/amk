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
#else
#   error "sd nand not available"
#endif