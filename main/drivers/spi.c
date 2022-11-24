 /**
  * spi.c
  */

#if defined(NRF52832_XXAA) || defined(NRF52840_XXAA)
#   include "spi_nrf52.c"
#elif defined(STM32F411xE) || defined(STM32F722xx) || defined(STM32F405xx) || defined(STM32F401xC) || defined(STM32F446xx) || defined(STM32F412Rx)
#   include "spi_stm32.c"
#elif defined(HC32F460)
#   include "spi_hc32.c"
#endif