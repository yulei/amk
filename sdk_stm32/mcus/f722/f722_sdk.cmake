
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

target_compile_options(${KEYBOARD}
    PRIVATE
    -mcpu=cortex-m7
    -mthumb
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    )

target_compile_definitions(${KEYBOARD} 
    PRIVATE 
    -DSPI_USE_INSTANCE_1
    -DSPI_USE_INSTANCE_2
    -DSTM32F722xx
    -DCFG_TUSB_MCU=OPT_MCU_STM32F7
    -DSYSTEM_CLOCK=216000000
    )

set(SDK_MCU_LD STM32F722RETx)
