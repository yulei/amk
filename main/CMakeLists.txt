
target_sources(${KEYBOARD}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/amk/amk_action.c
	${CMAKE_CURRENT_LIST_DIR}/amk/amk_keymap.c
	${CMAKE_CURRENT_LIST_DIR}/amk/amk_macro.c
	${CMAKE_CURRENT_LIST_DIR}/amk/amk_printf.c
	${CMAKE_CURRENT_LIST_DIR}/amk/amk_eeprom.c
	${CMAKE_CURRENT_LIST_DIR}/amk/amk_indicator.c
	${CMAKE_CURRENT_LIST_DIR}/amk/amk_profile.c
	${CMAKE_CURRENT_LIST_DIR}/amk/amk_boot.c
	${CMAKE_CURRENT_LIST_DIR}/amk/report_queue.c
	${CMAKE_CURRENT_LIST_DIR}/amk/debounce/debounce.c
)

target_include_directories(${KEYBOARD}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/amk
    ${CMAKE_CURRENT_LIST_DIR}/amk/debounce
    ${CMAKE_CURRENT_LIST_DIR}/porting
    ${CMAKE_CURRENT_LIST_DIR}/drivers
    ${CMAKE_CURRENT_LIST_DIR}/rgb
    ${CMAKE_CURRENT_LIST_DIR}/screen
    ${CMAKE_CURRENT_LIST_DIR}/usb
    ${CMAKE_CURRENT_LIST_DIR}/segger
)

if(RTOS_ENABLE)
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/rtos/usbx_main.c
        ${CMAKE_CURRENT_LIST_DIR}/rtos/tx_initialize_low_level.S
        ${CMAKE_CURRENT_LIST_DIR}/rtos/usbx_usb.c
        ${CMAKE_CURRENT_LIST_DIR}/rtos/usbx_desc.c
    )
    target_compile_definitions(${KEYBOARD} PRIVATE "RTOS_ENABLE")

    if(MCU_TYPE MATCHES "stm32f411")
        set(MCU_ARCH m4)
    endif()

    if(MCU_TYPE MATCHES "stm32f405")
        set(MCU_ARCH m4)
    endif()

    if(MCU_TYPE MATCHES "stm32f722")
        set(MCU_ARCH m7)
    endif()

    #include(${LIBS_DIR}/threadx.cmake)
    #include(${LIBS_DIR}/usbx.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/../libs/threadx.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/../libs/usbx.cmake)

else()
    target_sources(${KEYBOARD}
        PRIVATE
	    ${CMAKE_CURRENT_LIST_DIR}/amk/main.c
	    ${CMAKE_CURRENT_LIST_DIR}/usb/usb_descriptors.c
    )
    target_include_directories(${KEYBOARD}
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/../libs/tinyusb/src
    )
endif()

include(${CMAKE_CURRENT_LIST_DIR}/feature.cmake)

set(NRF5_MCUS "nrf52832" "nrf52840")
set(STM32_MCUS "stm32f722" "stm32f411" "stm32f405" "stm32f103" "stm32l072" )

if(${MCU_TYPE} IN_LIST STM32_MCUS)
    include(${CMAKE_CURRENT_LIST_DIR}/porting/wired.cmake)
endif()

if(${MCU_TYPE} IN_LIST NRF5_MCUS)
    include(${CMAKE_CURRENT_LIST_DIR}/porting/nrf52.cmake)
endif()

