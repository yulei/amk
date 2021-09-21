
set(MCU_FAMILY stm32)
set(MCU_TYPE stm32f722)
set(LD_PATH ${CMAKE_CURRENT_LIST_DIR})

add_executable(${KEYBOARD} onekey_keymap.c onekey.c)
target_include_directories(${KEYBOARD} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
