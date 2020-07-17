
set(NRF5_SRCS "${NRF5_SDK_PATH}/modules/nrfx/mdk/system_nrf52840.c" 
    "${NRF5_SDK_PATH}/modules/nrfx/mdk/gcc_startup_nrf52840.S")

add_library(nrf5_sdk STATIC ${NRF5_SRCS})

target_include_directories(nrf5_sdk PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${NRF5_SDK_PATH}/modules/nrfx
    ${NRF5_SDK_PATH}/modules/nrfx/mdk
    ${NRF5_SDK_PATH}/modules/nrfx/hal
    ${NRF5_SDK_PATH}/modules/nrfx/drivers/src
    ${NRF5_SDK_PATH}/integration/nrfx 
    ${NRF5_SDK_PATH}/components/toolchain/cmsis/include
    ${NRF5_SDK_PATH}/components/libraries/util
    ${NRF5_SDK_PATH}/components/softdevice/s140/headers)