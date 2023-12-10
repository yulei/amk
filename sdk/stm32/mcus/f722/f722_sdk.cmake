
target_sources(${KEYBOARD}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/startup_stm32f722xx.s
    ${CMAKE_CURRENT_LIST_DIR}/stm32f7xx_hal_timebase_tim.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_dma_ex.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_pwr_ex.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_i2c.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_i2c_ex.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_i2s.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_spi.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_spi_ex.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_iwdg.c
    )

set(MCU_FLAGS -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard CACHE STRING INTERNAL)

target_compile_definitions(${KEYBOARD} 
    PRIVATE 
    USE_SPI1
    USE_SPI2
    STM32F722xx
    CFG_TUSB_MCU=OPT_MCU_STM32F7
    SYSTEM_CLOCK=216000000
    )

set(SDK_MCU_LD STM32F722RETx)
