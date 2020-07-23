add_executable(amk "")

target_link_libraries(amk PRIVATE tusb tmk m20add)

target_include_directories(amk
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        porting
        rtt)

target_sources(amk
    PRIVATE         
        main.c 
        usb_descriptors.c
        matrix.c
        suspend.c
        xprintf.c)

target_sources(amk
    PRIVATE         
        rtt/SEGGER_RTT.c
        rtt/SEGGER_RTT_printf.c)

target_compile_definitions(amk
    PRIVATE
    SEGGER_RTT_ENABLE)

if (MCU_TYPE MATCHES "NRF52840")
    target_sources(amk
        PRIVATE 
            porting/nrf52/nrf52840.c
            porting/nrf52/timer.c
            porting/nrf52/bootloader.c
            porting/nrf52/gpio_pin.c
            porting/nrf52/wait.c)

    set(NRF5_MDK_PATH "${CMAKE_CURRENT_LIST_DIR}/../nrf5_sdk/nRF5_SDK_17.0.0_9d13099/modules/nrfx/mdk")
    target_link_options(amk PRIVATE "-L${NRF5_MDK_PATH}" "-T${CMAKE_CURRENT_LIST_DIR}/../nrf5_sdk/nrf52840_flash.ld")
    target_compile_definitions(amk PRIVATE MCU_NRF5X)
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
        ${CMAKE_SIZE_BIN} amk 
    )