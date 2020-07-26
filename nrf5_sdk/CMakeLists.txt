
set(NRF5_SDK_PATH "nRF5_SDK_17.0.0_9d13099")

add_library(nrf5_sdk STATIC "")
target_sources(nrf5_sdk
    PRIVATE
        ${NRF5_SDK_PATH}/components/libraries/log/src/nrf_log_backend_rtt.c
        ${NRF5_SDK_PATH}/components/libraries/log/src/nrf_log_backend_serial.c
        ${NRF5_SDK_PATH}/components/libraries/log/src/nrf_log_backend_uart.c
        ${NRF5_SDK_PATH}/components/libraries/log/src/nrf_log_default_backends.c
        ${NRF5_SDK_PATH}/components/libraries/log/src/nrf_log_frontend.c
        ${NRF5_SDK_PATH}/components/libraries/log/src/nrf_log_str_formatter.c
        ${NRF5_SDK_PATH}/components/libraries/timer/app_timer.c
        ${NRF5_SDK_PATH}/components/libraries/ringbuf/nrf_ringbuf.c
        ${NRF5_SDK_PATH}/components/libraries/atomic/nrf_atomic.c
        ${NRF5_SDK_PATH}/components/libraries/atomic_fifo/nrf_atfifo.c
        ${NRF5_SDK_PATH}/components/libraries/queue/nrf_queue.c
        ${NRF5_SDK_PATH}/components/libraries/memobj/nrf_memobj.c
        ${NRF5_SDK_PATH}/components/libraries/balloc/nrf_balloc.c
        ${NRF5_SDK_PATH}/components/libraries/sortlist/nrf_sortlist.c
        ${NRF5_SDK_PATH}/components/libraries/scheduler/app_scheduler.c
        ${NRF5_SDK_PATH}/components/libraries/util/app_error.c
        ${NRF5_SDK_PATH}/components/libraries/util/app_error_weak.c
        ${NRF5_SDK_PATH}/components/libraries/util/app_util_platform.c
        ${NRF5_SDK_PATH}/integration/nrfx/legacy/nrf_drv_clock.c
        ${NRF5_SDK_PATH}/modules/nrfx/drivers/src/nrfx_clock.c
        #${NRF5_SDK_PATH}/external/segger_rtt/SEGGER_RTT.c
        #${NRF5_SDK_PATH}/external/segger_rtt/SEGGER_RTT_printf.c
        )

if (MCU_TYPE MATCHES "NRF52840")
    target_sources(nrf5_sdk
        PRIVATE
            ${NRF5_SDK_PATH}/modules/nrfx/mdk/system_nrf52840.c 
            ${NRF5_SDK_PATH}/modules/nrfx/mdk/gcc_startup_nrf52840.S)

elseif (MCU_TYPE MATCHES "NRF52832")
    target_sources(nrf5_sdk
        PRIVATE
            ${NRF5_SDK_PATH}/modules/nrfx/mdk/system_nrf52.c 
            ${NRF5_SDK_PATH}/modules/nrfx/mdk/gcc_startup_nrf52.S)
else()
message(ERROR "could not find valid nrf chips")
endif()

target_include_directories(nrf5_sdk
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
        ${NRF5_SDK_PATH}/modules/nrfx
        ${NRF5_SDK_PATH}/modules/nrfx/mdk
        ${NRF5_SDK_PATH}/modules/nrfx/hal
        ${NRF5_SDK_PATH}/modules/nrfx/drivers/include
        ${NRF5_SDK_PATH}/modules/nrfx/drivers/src
        ${NRF5_SDK_PATH}/integration/nrfx 
        ${NRF5_SDK_PATH}/integration/nrfx/legacy
        ${NRF5_SDK_PATH}/components/toolchain/cmsis/include
        ${NRF5_SDK_PATH}/components/libraries/util
        ${NRF5_SDK_PATH}/components/libraries/atomic
        ${NRF5_SDK_PATH}/components/libraries/atomic_fifo
        ${NRF5_SDK_PATH}/components/libraries/queue
        ${NRF5_SDK_PATH}/components/libraries/cli
        ${NRF5_SDK_PATH}/components/libraries/timer
        ${NRF5_SDK_PATH}/components/libraries/log
        ${NRF5_SDK_PATH}/components/libraries/log/src
        ${NRF5_SDK_PATH}/components/libraries/memobj
        ${NRF5_SDK_PATH}/components/libraries/balloc
        ${NRF5_SDK_PATH}/components/libraries/delay
        ${NRF5_SDK_PATH}/components/libraries/strerror
        ${NRF5_SDK_PATH}/components/libraries/ringbuf
        ${NRF5_SDK_PATH}/components/libraries/sortlist
        ${NRF5_SDK_PATH}/components/libraries/scheduler
        ${NRF5_SDK_PATH}/components/libraries/experimental_section_vars
        ${NRF5_SDK_PATH}/components/softdevice/common
        ${NRF5_SDK_PATH}/components/softdevice/mbr/headers
        ${NRF5_SDK_PATH}/components/softdevice/s132/headers
        ${NRF5_SDK_PATH}/external/segger_rtt
        ${NRF5_SDK_PATH}/external/fprintf)