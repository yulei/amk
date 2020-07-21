#! /bin/sh

if [ $# -lt 2 ]; then
    echo "usage: run.sh env mcu"
else
    if [ $1 == "home" ]; then
        tp="/home/astro/arm/gcc-arm-none-eabi-9-2019-q4-major-win32"
    elif [ $1 == "work" ]; then
        tp="/home/leiyu/qmk_utils/gcc-arm-none-eabi"
    else 
        echo "Unknown enviroment, exiting..."
        exit
    fi

    if [ $2 == "nrf52" ]; then
        tf="./cmake/toolchain_nrf5x.cmake"
        mt="NRF52840"
    elif [ $2 == "stm32f4" ]; then
        tf="./cmake/toolchain_stm32f4.cmake"
        mt="STM32F411"
    else
        echo "Unknown enviroment, exiting..."
        exit
    fi
    cmake -S . -B build -DMCU_TYPE=${mt} -DCMAKE_TOOLCHAIN_FILE=${tf} -DTOOLCHAIN_PREFIX=${tp}
fi
