
cmake_minimum_required(VERSION 3.15.1)

# toolchain file
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/arm-none-eabi.cmake)

# project
project(amk VERSION 1.0.0)

enable_language(C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)


if(NOT DEFINED KEYBOARD)
    set(KEYBOARD onekey)
endif()


set(KEYBOARD_DIR ${CMAKE_CURRENT_LIST_DIR}/keyboards/${KEYBOARD} )

include(${KEYBOARD_DIR}/${KEYBOARD}.cmake)
set_target_properties(${KEYBOARD} PROPERTIES OUTPUT_NAME "${KEYBOARD}.elf")

set(SDK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sdk_${MCU_FAMILY})
add_subdirectory(${SDK_DIR})

set(MAIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/main)
add_subdirectory(${MAIN_DIR})

#always enabled tmk feature
set(MOUSEKEY_ENABLE yes)
set(EXTRAKEY_ENABLE yes)

set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
add_subdirectory(${LIBS_DIR})

message(STATUS "LDFLAGS:-L${LINKER_PATH} -T${LINKER_SCRIPT}")
target_compile_options(${KEYBOARD}
    PRIVATE
    ${MCU_FLAGS}
    -specs=nano.specs
    -Wall
    -Werror
    -ffunction-sections
    -fdata-sections
    -fno-strict-aliasing
    -fno-builtin
    -fshort-enums
    -fms-extensions
    )

target_link_options(${KEYBOARD}
    PRIVATE
    ${MCU_FLAGS}
    -L${LINKER_PATH}
    -T${LINKER_SCRIPT}
    -specs=nano.specs
    -specs=nosys.specs
    -lc
    -lm
    -lnosys
    -Wl,-Map=${KEYBOARD}.map,--cref
    -Wl,--gc-sections
    -Wl,--print-memory-usage 
    )


add_custom_command(TARGET ${KEYBOARD} POST_BUILD COMMAND ${OBJCOPY} -O ihex ${KEYBOARD}.elf ${KEYBOARD}.hex)
add_custom_command(TARGET ${KEYBOARD} POST_BUILD COMMAND ${OBJCOPY} -O binary ${KEYBOARD}.elf ${KEYBOARD}.bin)

add_custom_command(TARGET ${KEYBOARD} POST_BUILD COMMAND ${SIZE} ${KEYBOARD}.elf)