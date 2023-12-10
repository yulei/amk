
#pragma once

/* module define -------------------------------------------------------------*/
#define ADC_MODULE_ENABLED
//#define CAN_MODULE_ENABLED
//#define CRC_MODULE_ENABLED
//#define CRYP_MODULE_ENABLED
//#define DAC_MODULE_ENABLED
#define DBGMCU_MODULE_ENABLED
//#define DCI_MODULE_ENABLED
#define DMA_MODULE_ENABLED
//#define DMC_MODULE_ENABLED
//#define EINT_MODULE_ENABLED
#define FMC_MODULE_ENABLED
#define GPIO_MODULE_ENABLED
//#define HASH_MODULE_ENABLED
#define I2C_MODULE_ENABLED
//#define IWDT_MODULE_ENABLED
#define MISC_MODULE_ENABLED
#define PMU_MODULE_ENABLED
#define RCM_MODULE_ENABLED
//#define RNG_MODULE_ENABLED
#define RTC_MODULE_ENABLED
//#define SDIO_MODULE_ENABLED
//#define SMC_MODULE_ENABLED
#define SPI_MODULE_ENABLED
#define SYSCFG_MODULE_ENABLED
//#define TMR_MODULE_ENABLED
#define USART_MODULE_ENABLED
#define USB_MODULE_ENABLED
//#define WWDT_MODULE_ENABLED

/* includes ------------------------------------------------------------------*/
#ifdef ADC_MODULE_ENABLED
    #include "apm32f4xx_adc.h"
#endif

#ifdef CAN_MODULE_ENABLED
    #include "apm32f4xx_can.h"
#endif

#ifdef CRC_MODULE_ENABLED
    #include "apm32f4xx_crc.h"
#endif

#ifdef CRYP_MODULE_ENABLED
    #include "apm32f4xx_cryp.h"
#endif

#ifdef DAC_MODULE_ENABLED
    #include "apm32f4xx_dac.h"
#endif

#ifdef DBGMCU_MODUE_ENABLED
    #include "apm32f4xx_dbgmcu.h"
#endif

#ifdef DCI_MODULE_ENABLED
    #include "apm32f4xx_dci.h"
#endif

#ifdef DMA_MODULE_ENABLED
    #include "apm32f4xx_dma.h"
#endif

#ifdef DMC_MODULE_ENABLED
    #include "apm32f4xx_dmc.h"
#endif

#ifdef EINT_MODULE_ENABLED
    #include "apm32f4xx_eint.h"
#endif

#ifdef FMC_MODULE_ENABLED
    #include "apm32f4xx_fmc.h"
#endif

#ifdef GPIO_MODULE_ENABLED
    #include "apm32f4xx_gpio.h"
#endif

#ifdef HASH_MODULE_ENABLED
    #include "apm32f4xx_hash.h"
#endif

#ifdef I2C_MODULE_ENABLED
    #include "apm32f4xx_i2c.h"
#endif

#ifdef IWDT_MODULE_ENABLED
    #include "apm32f4xx_iwdt.h"
#endif

#ifdef MISC_MODULE_ENABLED
    #include "apm32f4xx_misc.h"
#endif

#ifdef PMU_MODULE_ENABLED
    #include "apm32f4xx_pmu.h"
#endif

#ifdef RCM_MODULE_ENABLED
    #include "apm32f4xx_rcm.h"
#endif

#ifdef RNG_MODULE_ENABLED
    #include "apm32f4xx_rng.h"
#endif

#ifdef RTC_MODULE_ENABLED
    #include "apm32f4xx_rtc.h"
#endif

#ifdef SDIO_MODULE_ENABLED
    #include "apm32f4xx_sdio.h"
#endif

#ifdef SMC_MODULE_ENABLED
    #include "apm32f4xx_smc.h"
#endif

#ifdef SPI_MODULE_ENABLED
    #include "apm32f4xx_spi.h"
#endif

#ifdef SYSCFG_MODULE_ENABLED
    #include "apm32f4xx_syscfg.h"
#endif

#ifdef TMR_MODULE_ENABLED
    #include "apm32f4xx_tmr.h"
#endif

#ifdef USART_MODULE_ENABLED
    #include "apm32f4xx_usart.h"
#endif

#ifdef USB_MODULE_ENABLED
    #include "apm32f4xx_usb_device.h"
    #include "apm32f4xx_usb.h"
#endif

#ifdef WWDT_MODULE_ENABLED
    #include "apm32f4xx_wwdtg.h"
#endif
