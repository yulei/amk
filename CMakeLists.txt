cmake_minimum_required(VERSION 3.15)

project(astro VERSION 0.0.1
            DESCRIPTION "Keyboard firmware"
            LANGUAGES C ASM)


#add_subdirectory(nrf5_sdk)
add_subdirectory(stm32_sdk)
add_subdirectory(tinyusb)
add_subdirectory(tmk)
add_subdirectory(main)
add_subdirectory(keyboards)