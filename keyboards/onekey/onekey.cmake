
set(MCU_FAMILY stm32)
set(MCU_TYPE stm32f722)
set(LD_PATH ${CMAKE_CURRENT_LIST_DIR})

set(TINYUSB_ENABLE yes)
set(TINYUSB_USE_HAL yes)

add_executable(${KEYBOARD} ${KEYBOARD_DIR}/onekey_keymap.c ${KEYBOARD_DIR}/onekey.c)
target_include_directories(${KEYBOARD} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
