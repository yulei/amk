
set(MCU_FAMILY stm32)
set(MCU_TYPE stm32f411)
set(LINKER_PATH ${CMAKE_CURRENT_LIST_DIR} CACHE STRING INTERNAL)

set(TINYUSB_ENABLE yes)
set(TINYUSB_USE_HAL yes)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cortex_m4.cmake)

add_executable(${KEYBOARD} ${KEYBOARD_DIR}/onekey_keymap.c ${KEYBOARD_DIR}/onekey.c)
target_include_directories(${KEYBOARD} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
