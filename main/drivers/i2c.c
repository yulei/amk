 /**
  * i2c.c
  */

#if defined(NRF52832_XXAA) || defined(NRF52840_XXAA)
#   include "i2c_nrf52.c"
#elif defined(STM32F411xE)
#   include "i2c_stm32.c"
#endif