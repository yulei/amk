/**
 * @file qspi.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#if defined(HC32F460)
#   include "qspi_hc32.c"
#else
#   error "qspi not available"
#endif