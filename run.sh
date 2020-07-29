#! /bin/sh

if [ $# -lt 1 ]; then
    echo "usage: run.sh mcu"
else
    if [ $1 == "nrf52832" ]; then
        tf="./cmake/toolchain_nrf5x.cmake"
        mt="NRF52832"
    elif [ $1 == "nrf52840" ]; then
        tf="./cmake/toolchain_nrf5x.cmake"
        mt="NRF52840"
    elif [ $1 == "stm32f4" ]; then
        tf="./cmake/toolchain_stm32f4.cmake"
        mt="STM32F411"
    else
        echo "Unknown mcu, exiting..."
        exit
    fi
    cmake -S . -B build -DMCU_TYPE=${mt} -DCMAKE_TOOLCHAIN_FILE=${tf}
fi
