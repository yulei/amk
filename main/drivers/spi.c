 /**
  * spi.c
  */

#if defined(NRF52832_XXAA) || defined(NRF52840_XXAA)
#   include "spi_nrf52.c"
#elif defined(STM32F411xE) || defined(STM32L433xx) || defined(STM32F722xx)
#   include "spi_stm32.c"
#endif