 /**
  * i2c.c
  */

#if defined(NRF52832_XXAA) || defined(NRF52840_XXAA)
#   include "i2c_nrf52.c"
#elif defined(STM32F411xE) || defined(STM32F405xx) || defined(STM32F722xx) || defined(STM32L072xx) || defined(STM32F401xC) || defined(STM32F446xx)
#   include "i2c_stm32.c"
#elif defined(HC32F460xE)
#   include "i2c_hc32.c"
#endif