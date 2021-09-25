
set(MCU_FAMILY nrf5)
set(MCU_TYPE nrf52840)
set(TINYUSB_ENABLE yes)

add_executable(${KEYBOARD} 
    ${KEYBOARD_DIR}/rfhost_keymap.c
    ${KEYBOARD_DIR}/rfhost_matrix.c
    ${KEYBOARD_DIR}/rfhost.c
    )

target_include_directories(${KEYBOARD} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
