
add_executable(kbd main.c)

target_link_libraries(kbd PRIVATE tusb)


if (MCU_TYPE STREQUAL "NRF52840")
  set(NRF5_MDK_PATH "${CMAKE_CURRENT_LIST_DIR}/../nrf5_sdk/nRF5_SDK_17.0.0_9d13099/modules/nrfx/mdk")
  target_link_options(kbd PRIVATE "-L${NRF5_MDK_PATH}" "-T${NRF5_MDK_PATH}/nrf52840_xxaa.ld")
elseif(MCU_TYPE STREQUAL "STM32F411")
  set(STM32F4_LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/../stm32_sdk/stm32f411ceux_flash.ld")
  target_link_options(kbd PRIVATE "-T${STM32F4_LINKER_SCRIPT}")
else()
  message(ERROR "unknown platform on main application building")
endif()
