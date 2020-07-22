add_executable(amk "")

set(CONFIG_INCLUDE_PATH ../keyboards/m20add)
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
    SEGGER_RTT_ENABLE
    CFG_TUSB_MCU=OPT_MCU_STM32F4)

if (MCU_TYPE STREQUAL "NRF52840")
    set(NRF5_MDK_PATH "${CMAKE_CURRENT_LIST_DIR}/../nrf5_sdk/nRF5_SDK_17.0.0_9d13099/modules/nrfx/mdk")
    target_link_options(amk PRIVATE "-L${NRF5_MDK_PATH}" "-T${NRF5_MDK_PATH}/nrf52840_xxaa.ld")
elseif(MCU_TYPE STREQUAL "STM32F411")
    target_sources(amk
        PRIVATE 
            porting/stm32/generic_stm32f4.c
            porting/stm32/timer.c
            porting/stm32/bootloader.c
            porting/stm32/gpio_pin.c
            porting/stm32/wait.c)

    set(STM32F4_LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/../stm32_sdk/stm32f411ceux_flash.ld")
    target_link_options(amk PRIVATE "-T${STM32F4_LINKER_SCRIPT}")
else()
    message(ERROR "unknown platform on main application building")
endif()
