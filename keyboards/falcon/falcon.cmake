
set(MCU_FAMILY nrf5)
set(MCU_TYPE nrf52832)

set(EECONFIG_FLASH yes)
set(RGB_LINEAR_ENABLE yes)

add_executable(${KEYBOARD} ${KEYBOARD_DIR}/falcon.c ${KEYBOARD_DIR}/falcon_keymap.c)
target_include_directories(${KEYBOARD} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
