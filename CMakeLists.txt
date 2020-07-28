cmake_minimum_required(VERSION 3.15)

project(astro
      VERSION 0.0.1
      DESCRIPTION "Keyboard firmware"
      LANGUAGES C ASM)

set(CONFIG_INCLUDE_PATH "${CMAKE_CURRENT_LIST_DIR}/keyboards/hhkbble")

if(NOT MCU_TYPE)
    set(MCU_TYPE "NRF52840" CACHE STRING "mcu chip type")
endif()

if (MCU_TYPE STREQUAL "NRF52840")
  message(STATUS "building on nrf platform for NRF52840")
  add_subdirectory(nrf5_sdk)
  add_subdirectory(tinyusb)
elseif (MCU_TYPE STREQUAL "NRF52832")
  message(STATUS "building on nrf platform for NRF52832")
  add_subdirectory(nrf5_sdk)
elseif(MCU_TYPE STREQUAL "STM32F411")
  message(STATUS "building on stm32 platform for STM32F411")
  add_subdirectory(stm32_sdk)
  add_subdirectory(tinyusb)
else()
  message(ERROR " Unknown mcu chip type: ${MCU_TYPE}")
endif()

add_subdirectory(keyboards)
add_subdirectory(tmk)
add_subdirectory(main)