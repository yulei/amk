
set(MCU_FAMILY stm32)
set(MCU_TYPE stm32f411)
#set(MCU_TYPE stm32f103)
#set(MCU_TYPE stm32l072)
set(LINKER_PATH ${CMAKE_CURRENT_LIST_DIR} CACHE STRING INTERNAL)

add_executable(${KEYBOARD} 
    ${KEYBOARD_DIR}/usbmaster.c
    ${KEYBOARD_DIR}/ring_buffer.c
    ${KEYBOARD_DIR}/display.c
    )

target_include_directories(${KEYBOARD}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    )
target_compile_definitions(${KEYBOARD}
    PRIVATE
    VITA
    #ABELX
    #ADD
    #FALCON
    )

set(CUSTOM_MATRIX yes)
set(TINYUSB_ENABLE yes)
set(TINYUSB_USE_HAL yes)
set(ACTIONMAP_ENABLE yes)
#set(DATETIME_ENABLE yes)
#set(RTC_DRIVER bl5372)
set(SCREEN_ENABLE yes)
set(MSC_ENABLE yes)
#set(RTOS_ENABLE yes)