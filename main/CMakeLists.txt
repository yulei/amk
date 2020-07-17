
add_executable(kbd main.c)

target_link_libraries(kbd PRIVATE tusb)
#set(NRF5_LINKER_SCRIPT "${NRF5_SDK_PATH}/modules/nrfx/mdk/nrf52840_xxaa.ld")
#target_link_options(kbd PRIVATE "-L${NRF5_SDK_PATH}/modules/nrfx/mdk" "-T${NRF5_LINKER_SCRIPT}")

set(STM32F4_LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/../stm32_sdk/stm32f411ceux_flash.ld")
target_link_options(kbd PRIVATE "-T${STM32F4_LINKER_SCRIPT}")