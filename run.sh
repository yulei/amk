#! /bin/sh

cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="./cmake/toolchain_nrf5x.cmake" -DTOOLCHAIN_PREFIX="/home/leiyu/qmk_utils/gcc-arm-none-eabi"
