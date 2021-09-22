
set(MCU_FAMILY stm32)
set(MCU_TYPE stm32f405)
set(LINKER_PATH ${CMAKE_CURRENT_LIST_DIR} CACHE STRING INTERNAL)

set(USB_HOST_ENABLE yes)
set(RGB_MATRIX_ENABLE yes)
set(RGB_LINEAR_ENABLE yes)
set(EECONFIG_FLASH yes)

add_executable(${KEYBOARD} ${KEYBOARD_DIR}/pad_keymap.c ${KEYBOARD_DIR}/pad.c)
target_include_directories(${KEYBOARD} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
