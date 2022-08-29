/**
 * @file ws2812.c
 */

#ifdef WS2812_LED_NUM
#if defined(NRF52832_XXAA) || defined(NRF52840_XXAA)
#   include "ws2812_nrf52.c"
#elif defined(HC32F460)
#   include "ws2812_hc32_spi.c"
#elif defined(GD32E10X) || defined(GD32E50X)
#   include "ws2812_gd32_pwm.c"
#else
#ifndef WS2812_USE_GPIO
#   include "ws2812_stm32_pwm.c"
#else
#   include "ws2812_stm32_gpio.c"
#endif
#endif
#endif
