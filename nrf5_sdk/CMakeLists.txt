
set(NRF5_SDK_PATH "nRF5_SDK_17.0.0_9d13099")

add_library(nrf5_sdk STATIC "")
target_sources(nrf5_sdk
        PRIVATE
          ${NRF5_SDK_PATH}/modules/nrfx/mdk/system_nrf52840.c 
          ${NRF5_SDK_PATH}/modules/nrfx/mdk/gcc_startup_nrf52840.S
        )

target_include_directories(nrf5_sdk
        PUBLIC 
          ${CMAKE_CURRENT_LIST_DIR}
          ${NRF5_SDK_PATH}/modules/nrfx
          ${NRF5_SDK_PATH}/modules/nrfx/mdk
          ${NRF5_SDK_PATH}/modules/nrfx/hal
          ${NRF5_SDK_PATH}/modules/nrfx/drivers/src
          ${NRF5_SDK_PATH}/integration/nrfx 
          ${NRF5_SDK_PATH}/components/toolchain/cmsis/include
          ${NRF5_SDK_PATH}/components/libraries/util
          ${NRF5_SDK_PATH}/components/softdevice/s140/headers
        )