/**
 * @file ws2812.c
 */

#if defined(NRF52832_XXAA) || defined(NRF52840_XXAA)
#   include "ws2812_nrf52.c"
#else
#   include "ws2812_stm32.c"
#endif
