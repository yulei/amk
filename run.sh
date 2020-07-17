#! /bin/sh

#cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="./cmake/toolchain_nrf5x.cmake" -DTOOLCHAIN_PREFIX="/home/leiyu/qmk_utils/gcc-arm-none-eabi"
#cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="./cmake/toolchain_nrf5x.cmake" -DTOOLCHAIN_PREFIX="/home/astro/arm/gcc-arm-none-eabi-9-2019-q4-major-win32"
cmake -S . -B build -DSTM32_CHIP=STM32F411CE -DCMAKE_TOOLCHAIN_FILE="./cmake/toolchain_stm32f4.cmake" -DTOOLCHAIN_PREFIX="/home/leiyu/qmk_utils/gcc-arm-none-eabi"
#cmake -S . -B build -DSTM32_CHIP=STM32F411CE -DCMAKE_TOOLCHAIN_FILE="./cmake/toolchain_stm32f4.cmake" -DTOOLCHAIN_PREFIX="/home/astro/arm/gcc-arm-none-eabi-9-2019-q4-major-win32"