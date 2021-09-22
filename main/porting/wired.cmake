
target_sources(${KEYBOARD}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../segger/SEGGER_RTT.c
    ${CMAKE_CURRENT_LIST_DIR}/wired/common/bootloader.c
    ${CMAKE_CURRENT_LIST_DIR}/wired/common/generic_board.c
    ${CMAKE_CURRENT_LIST_DIR}/wired/common/suspend.c
    ${CMAKE_CURRENT_LIST_DIR}/wired/common/timer.c
    )

target_include_directories(${KEYBOARD}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/wired/common
    )

if(NOT RTOS_ENABLE)
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/wired/common/usb_interface.c
        )
endif()

if(TINYUSB_ENABLE)
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/wired/common/usb_tinyusb.c
        )
endif()

include(${CMAKE_CURRENT_LIST_DIR}/wired/${MCU_FAMILY}.cmake)