#add_executable(nrf52_kbd nrf52kbd.c)
#target_link_libraries(nrf52_kbd PRIVATE nrf5_sdk)

add_executable(amk "")
set_target_properties(
    amk 
    PROPERTIES 
        SUFFIX ".elf")

target_link_libraries(amk PRIVATE tmk kbd)

target_include_directories(amk
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/../tinyusb
        ${CMAKE_CURRENT_LIST_DIR}/../tinyusb/tinyusb/src
        porting
        porting/nrf52
        porting/nrf52/ble
        rtt)

target_sources(amk
    PRIVATE         
        main.c 
        matrix_driver.c
        suspend.c
        xprintf.c)

#target_sources(amk
#    PRIVATE         
#        rtt/SEGGER_RTT.c
#        rtt/SEGGER_RTT_printf.c)

target_compile_definitions(amk
    PRIVATE
    SEGGER_RTT_ENABLE
    CFG_TUSB_MCU=OPT_MCU_NRF5X)

if (MCU_TYPE MATCHES "NRF52840")
    target_sources(amk
        PRIVATE 
            porting/nrf52/nrf52840.c
            porting/nrf52/timer.c
            porting/nrf52/bootloader.c
            porting/nrf52/gpio_pin.c
            porting/nrf52/wait.c
            porting/nrf52/eeconfig_fds.c)

    set(NRF5_MDK_PATH "${CMAKE_CURRENT_LIST_DIR}/../nrf5_sdk/nRF5_SDK_17.0.0_9d13099/modules/nrfx/mdk")
    target_link_options(amk PRIVATE "-L${NRF5_MDK_PATH}" "-T${CMAKE_CURRENT_LIST_DIR}/../nrf5_sdk/nrf52840.ld")
    target_compile_definitions(amk PRIVATE MCU_NRF5X)
elseif (MCU_TYPE MATCHES "NRF52832")
    target_sources(amk
        PRIVATE 
            porting/nrf52/timer.c
            porting/nrf52/bootloader.c
            porting/nrf52/gpio_pin.c
            porting/nrf52/wait.c
            #porting/nrf52/eeconfig_mem.c)
            porting/nrf52/eeconfig_fds.c)

     target_sources(amk
        PRIVATE 
            porting/nrf52/ble/ble_adv_service.c
            porting/nrf52/ble/ble_bas_service.c
            porting/nrf52/ble/ble_hids_service.c
            porting/nrf52/ble/ble_services.c)

    target_sources(amk
        PRIVATE 
            porting/nrf52/nrf52832.c
            porting/nrf52/ble_keyboard.c
            porting/nrf52/usb_backend_uart.c)

    target_link_libraries(amk PRIVATE nrf5_sdk)

    set(NRF5_MDK_PATH "${CMAKE_CURRENT_LIST_DIR}/../nrf5_sdk/nRF5_SDK_17.0.0_9d13099/modules/nrfx/mdk")
    target_link_options(amk PRIVATE "-L${NRF5_MDK_PATH}" "-T${CMAKE_CURRENT_LIST_DIR}/../nrf5_sdk/nrf52832.ld")
    target_compile_definitions(amk PRIVATE MCU_NRF5X MATRIX_USE_GPIO)

    if (MMD) 
        target_sources(amk
            PRIVATE 
                porting/nrf52/mmd/JLINK_MONITOR.c
                porting/nrf52/mmd/JLINK_MONITOR_ISR_SES.s)
        target_include_directories(amk
            PRIVATE 
                porting/nrf52/mmd)
        target_compile_options(amk
            PRIVATE
                CONFIG_JLINK_MONITOR_ENABLED)
    endif()


    #target_link_options(nrf52_kbd PRIVATE "-L${NRF5_MDK_PATH}" "-T${CMAKE_CURRENT_LIST_DIR}/../nrf5_sdk/nrf52832.ld")
elseif(MCU_TYPE MATCHES "STM32F411")
    target_sources(amk
        PRIVATE 
            porting/stm32/generic_stm32f4.c
            porting/stm32/timer.c
            porting/stm32/bootloader.c
            porting/stm32/gpio_pin.c
            porting/stm32/wait.c)

    set(STM32F4_LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/../stm32_sdk/stm32f411xe_flash.ld")
    target_link_options(amk PRIVATE "-T${STM32F4_LINKER_SCRIPT}")
    target_compile_definitions(amk PRIVATE MCU_STM32F4)
else()
    message(ERROR "unknown platform on main application building")
endif()

set(TARGET_FULLNAME $<TARGET_FILE_NAME:amk>)
get_filename_component(TARGET_BASENAME ${TARGET_FULLNAME} NAME_WE)

add_custom_command(
    TARGET
        amk
    POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E  echo "Converting to hex ..."
    COMMAND
        ${CMAKE_OBJCOPY_BIN} -O ihex  ${TARGET_FULLNAME} ${TARGET_BASENAME}.hex
    COMMAND
        ${CMAKE_COMMAND} -E  echo "Converting to binary ..."
    COMMAND
        ${CMAKE_OBJCOPY_BIN} -O binary ${TARGET_FULLNAME} ${TARGET_BASENAME}.bin
    COMMAND
        ${CMAKE_COMMAND} -E  echo "Final size summary:"
    COMMAND
        ${CMAKE_SIZE_BIN} ${TARGET_FULLNAME}
    )