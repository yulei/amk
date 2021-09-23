

target_sources(${KEYBOARD}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/nrf52/generic_nrf52.c
    ${CMAKE_CURRENT_LIST_DIR}/nrf52/timer.c
    ${CMAKE_CURRENT_LIST_DIR}/nrf52/bootloader.c
    ${CMAKE_CURRENT_LIST_DIR}/nrf52/gpio_pin.c
    ${CMAKE_CURRENT_LIST_DIR}/nrf52/suspend.c
    ${CMAKE_CURRENT_LIST_DIR}/nrf52/wait.c
    ${CMAKE_CURRENT_LIST_DIR}/nrf52/ble_keyboard.c
    ${CMAKE_CURRENT_LIST_DIR}/nrf52/gzll_keyboard.c
    ${CMAKE_CURRENT_LIST_DIR}/nrf52/rf_keyboard.c
    ${CMAKE_CURRENT_LIST_DIR}/nrf52/rf_power.c
    ${CMAKE_CURRENT_LIST_DIR}/nrf52/ble/ble_adv_service.c
    ${CMAKE_CURRENT_LIST_DIR}/nrf52/ble/ble_bas_service.c
    ${CMAKE_CURRENT_LIST_DIR}/nrf52/ble/ble_hids_service.c
    ${CMAKE_CURRENT_LIST_DIR}/nrf52/ble/ble_services.c
    )

target_include_directories(${KEYBOARD}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/nrf52
    ${CMAKE_CURRENT_LIST_DIR}/nrf52/ble
    ${CMAKE_CURRENT_LIST_DIR}/../../libs/tinyusb/src
    )

if(JLINK_MMD)
    target_sources(${KEYBOARD}
        PRIVATE
    	${CMAKE_CURRENT_LIST_DIR}/nrf52/mmd/JLINK_MONITOR_ISR_SES.c
    	${CMAKE_CURRENT_LIST_DIR}/nrf52/mmd/JLINK_MONITOR.c
        )
	target_include_directories(${KEYBOARD}
		PUBLIC
		${CMAKE_CURRENT_LIST_DIR}/nrf52/mmd
	)
	target_compile_definitions(${KEYBOARD} PRIVATE "CONFIG_JLINK_MONITOR_ENABLED=1")
endif()

if(MCU_TYPE MATCHES "nrf52832")
    target_sources(${KEYBOARD}
        PRIVATE
    	${CMAKE_CURRENT_LIST_DIR}/nrf52/usb_backend_uart.c
		)
endif()

if(MCU_TYPE MATCHES "nrf52840")
    target_sources(${KEYBOARD}
        PRIVATE
    	${CMAKE_CURRENT_LIST_DIR}/nrf52/usb_backend_tinyusb.c
		)
endif()
