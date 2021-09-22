
target_sources(${KEYBOARD}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/stm32/gpio_pin.c
    ${CMAKE_CURRENT_LIST_DIR}/stm32/stm32.c
    ${CMAKE_CURRENT_LIST_DIR}/stm32/wait.c
    )

target_include_directories(${KEYBOARD}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/stm32
    )

if(NOT TINYUSB_ENABLE AND NOT RTOS_ENABLE)
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/stm32/usb_hal.c
    )
endif()

if(MCU_TYPE STREQUAL stm32f411)
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/stm32/stm32f411xe.c
    )
endif()

if(MCU_TYPE STREQUAL stm32f405)
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/stm32/stm32f405xx.c
    )
endif()

if(MCU_TYPE STREQUAL stm32f722)
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/stm32/stm32f722xx.c
    )
endif()

if(MCU_TYPE STREQUAL stm32f103)
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/stm32/stm32f103xb.c
    )
endif()

if(MCU_TYPE STREQUAL stm32l432)
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/stm32/stm32l432xx.c
    )
endif()

if(MCU_TYPE STREQUAL stm32l072)
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/stm32/stm32l072xx.c
    )
endif()