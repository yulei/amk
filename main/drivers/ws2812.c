/**
 * @file ws2812.c
 */

#if defined(NRF52832_XXAA)
#   include "ws2812_nrf52.c"
#elif defined(STM32F411xE)
#   include "ws2812_stm32.c"
#endif
