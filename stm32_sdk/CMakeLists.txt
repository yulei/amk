
add_library(stm32_sdk STATIC "")

target_include_directories(stm32_sdk 
        PUBLIC
          ${CMAKE_CURRENT_LIST_DIR}
          STM32F4xx_HAL_Driver/Inc 
          STM32F4xx_HAL_Driver/Inc/Legacy 
          CMSIS/Device/ST/STM32F4xx/Include
          CMSIS/Include
        )

target_sources(stm32_sdk
        PRIVATE
          system_stm32f4xx.c
          startup_stm32f411xe.s
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc_ex.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c
          STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c
        )     