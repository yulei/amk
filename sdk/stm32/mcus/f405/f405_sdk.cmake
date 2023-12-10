
target_sources(${KEYBOARD}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/startup_stm32f405xx.s
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_flash_ramfunc.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_dma_ex.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_pwr_ex.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_i2c.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_i2c_ex.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_i2s.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_tim.c
    )

set(MCU_FLAGS -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16 CACHE STRING INTERNAL)

target_compile_definitions(${KEYBOARD} 
    PRIVATE 
    STM32F405xx 
    CFG_TUSB_MCU=OPT_MCU_STM32F4
    SYSTEM_CLOCK=168000000
    )

set(SDK_MCU_LD STM32F405RGTx)
