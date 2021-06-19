 /**
  * i2c.c
  */

#if defined(NRF52832_XXAA) || defined(NRF52840_XXAA)
#   include "i2c_nrf52.c"
#elif defined(STM32F411xE) || defined(STM32F405xx) || defined(STM32F722xx)
#   include "i2c_stm32.c"
#endif