if(stm32f722 STREQUAL ${MCU_TYPE})
    set(SDK_MCU_SERIES f7)
    set(SDK_MCU_FAMILY stm32f7xx)
    set(SDK_MCU_TYPE f722)
endif()

set(VENDOR_DIR ${CMAKE_CURRENT_LIST_DIR}/../libs/vendor/st)

target_sources(${KEYBOARD}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/mcus/${SDK_MCU_TYPE}/system_${SDK_MCU_FAMILY}.c
    ${CMAKE_CURRENT_LIST_DIR}/mcus/${SDK_MCU_TYPE}/${SDK_MCU_FAMILY}_hal_msp.c
    ${CMAKE_CURRENT_LIST_DIR}/mcus/${SDK_MCU_TYPE}/${SDK_MCU_FAMILY}_it.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_tim.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_tim_ex.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_rcc.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_rcc_ex.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_flash.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_flash_ex.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_gpio.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_dma.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_pwr.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_rtc.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_rtc_ex.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_cortex.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_exti.c
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_uart.c
    )

target_include_directories(${KEYBOARD}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/mcus/${SDK_MCU_TYPE}
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Inc
    ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Inc/Legacy
    ${VENDOR_DIR}/device_${SDK_MCU_SERIES}/Include
    ${VENDOR_DIR}/cmsis/Core/Include
    ${VENDOR_DIR}/cmsis/DSP/Include
    )

target_compile_definitions(${KEYBOARD} PRIVATE "USE_HAL_DRIVER")

include(${CMAKE_CURRENT_LIST_DIR}/mcus/${SDK_MCU_TYPE}/${SDK_MCU_TYPE}_sdk.cmake)

if(${TINYUSB_ENABLE} AND ${TINYUSB_USE_HAL})
    target_sources(${KEYBOARD}
        PRIVATE
        ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_pcd.c
        ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_pcd_ex.c
        ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_ll_usb.c
        )
    target_compile_definitions(${KEYBOARD} PRIVATE "HAL_PCD_MODULE_ENABLED")
else ()
#	ifneq (yes, $(strip $(RTOS_ENABLE)))
#		include $(STM32SDK_DIR)/hal_usb.mk
#	endif
#	SRCS += $(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pcd.c 
#	SRCS += $(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pcd_ex.c
#	SRCS += $(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_ll_usb.c 
#	APP_DEFS += -DHAL_PCD_MODULE_ENABLED
endif()

if( NOT DEFINED ${LINKER_PATH})
    set(LINKER_PATH	${CMAKE_CURRENT_LIST_DIR}/mcus/linker PARENT_SCOPE)
endif()

set(LINKER_SCRIPT ${SDK_MCU_LD}_FLASH.ld PARENT_SCOPE)

message(STATUS "linker_path: ${LINKER_PATH}")
message(STATUS "linker_script: ${LINKER_SCRIPT}")
