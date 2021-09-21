
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

message(STATUS "MCU_TYPE=${MCU_TYPE}")
if(NOT ${TINYUSB_ENABLE} AND NOT ${RTOS_ENABLE})
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/stm32/usb_hal.c
    )
endif()

if(stm32f411 STREQUAL ${MCU_TYPE})
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/stm32/stm32f411xe.c
    )
endif()

if(stm32f405 STREQUAL ${MCU_TYPE})
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/stm32/stm32f405xx.c
    )
endif()

if(stm32f722 STREQUAL ${MCU_TYPE})
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/stm32/stm32f722xx.c
    )
    message(STATUS "use stm32f722xx.c")
endif()

if(stm32f103 STREQUAL ${MCU_TYPE})
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/stm32/stm32f103xb.c
    )
endif()

if(stm32l432 STREQUAL ${MCU_TYPE})
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/stm32/stm32l432xx.c
    )
endif()

if(stm32l072 STREQUAL ${MCU_TYPE})
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/stm32/stm32l072xx.c
    )
endif()