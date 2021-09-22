if(MCU_TYPE STREQUAL "stm32f722")
    set(SDK_MCU_SERIES f7)
    set(SDK_MCU_FAMILY stm32f7xx)
    set(SDK_MCU_TYPE f722)
endif()

if(MCU_TYPE STREQUAL "stm32f405")
    set(SDK_MCU_SERIES f4)
    set(SDK_MCU_FAMILY stm32f4xx)
    set(SDK_MCU_TYPE f405)
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

if(NOT TINYUSB_ENABLE} OR TINYUSB_USE_HAL)
    target_sources(${KEYBOARD}
        PRIVATE
        ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_pcd.c
        ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_pcd_ex.c
        ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_ll_usb.c
        )
    target_compile_definitions(${KEYBOARD} PRIVATE "HAL_PCD_MODULE_ENABLED")
endif()

if(NOT TINYUSB_ENABLE AND NOT RTOS_ENABLE)
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/hal_usb/usb_device.c
        ${CMAKE_CURRENT_LIST_DIR}/hal_usb/usbd_conf.c
        ${CMAKE_CURRENT_LIST_DIR}/hal_usb/usbd_desc.c
        ${CMAKE_CURRENT_LIST_DIR}/hal_usb/usbd_composite.c
        ${VENDOR_DIR}/usb_device/Core/Src/usbd_core.c
        ${VENDOR_DIR}/usb_device/Core/Src/usbd_ctlreq.c
        ${VENDOR_DIR}/usb_device/Core/Src/usbd_ioreq.c
    )
    if(USB_HOST_ENABLE)
        target_sources(${KEYBOARD}
            PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/hal_usb/report_parser.c
            ${CMAKE_CURRENT_LIST_DIR}/hal_usb/usbh_hid_multi.c
            ${CMAKE_CURRENT_LIST_DIR}/hal_usb/usb_host.c
            ${CMAKE_CURRENT_LIST_DIR}/hal_usb/usbh_conf.c
            ${VENDOR_DIR}/usb_host/Core/Src/usbh_core.c
            ${VENDOR_DIR}/usb_host/Core/Src/usbh_ctlreq.c
            ${VENDOR_DIR}/usb_host/Core/Src/usbh_ioreq.c
            ${VENDOR_DIR}/usb_host/Core/Src/usbh_pipes.c
            ${VENDOR_DIR}/usb_host/Core/Src/usbh_pipes.c
            ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_hal_hcd.c
            ${VENDOR_DIR}/driver_${SDK_MCU_SERIES}/Src/${SDK_MCU_FAMILY}_ll_usb.c
        )
        target_compile_definitions(${KEYBOARD} 
            PRIVATE 
            -DUSB_HOST_ENABLE
            -DHAL_HCD_MODULE_ENABLED
            )
        target_include_directories(${KEYBOARD}
            PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/hal_usb
            ${VENDOR_DIR}/usb_device/Core/Inc
            ${VENDOR_DIR}/usb_host/Core/Inc
            ${CMAKE_CURRENT_LIST_DIR}/../libs/tinyusb/src 
        )
    endif()
endif()

if(NOT DEFINED LINKER_PATH)
    set(LINKER_PATH	${CMAKE_CURRENT_LIST_DIR}/mcus/linker CACHE STRING INTERNAL)
endif()

set(LINKER_SCRIPT ${SDK_MCU_LD}_FLASH.ld CACHE STRING INTERNAL)
