
target_sources(${KEYBOARD}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/amk/amk_action.c
	${CMAKE_CURRENT_LIST_DIR}/amk/amk_keymap.c
	${CMAKE_CURRENT_LIST_DIR}/amk/amk_macro.c
	${CMAKE_CURRENT_LIST_DIR}/amk/amk_printf.c
	${CMAKE_CURRENT_LIST_DIR}/amk/amk_eeprom.c
	${CMAKE_CURRENT_LIST_DIR}/amk/amk_boot.c
	${CMAKE_CURRENT_LIST_DIR}/amk/report_queue.c
	${CMAKE_CURRENT_LIST_DIR}/usb/usb_descriptors.c
)

target_include_directories(${KEYBOARD}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/amk
    ${CMAKE_CURRENT_LIST_DIR}/porting
    ${CMAKE_CURRENT_LIST_DIR}/drivers
    ${CMAKE_CURRENT_LIST_DIR}/rgb
    ${CMAKE_CURRENT_LIST_DIR}/screen
    ${CMAKE_CURRENT_LIST_DIR}/usb
    ${CMAKE_CURRENT_LIST_DIR}/segger
    ${LIB_DIR}/tinyusb/src
)

if(${RTOS_ENABLE})
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/rtos/usbx_main.c
        ${CMAKE_CURRENT_LIST_DIR}/rtos/tx_initialize_low_level.S
        ${CMAKE_CURRENT_LIST_DIR}/rtos/usbx_usb.c
        ${CMAKE_CURRENT_LIST_DIR}/rtos/usbx_desc.c
    )
    target_compile_definitions(${KEYBOARD} public "RTOS_ENABLE")

    #include ${LIBS_DIR}/threadx.cmake
    #include ${LIBS_DIR}/usbx.cmake

else()
    target_sources(${KEYBOARD}
        PRIVATE
	    ${CMAKE_CURRENT_LIST_DIR}/amk/main.c
    )
endif()

include(${CMAKE_CURRENT_LIST_DIR}/feature.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/porting/wired.cmake)
