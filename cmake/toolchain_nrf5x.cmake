# Target definition
set(CMAKE_SYSTEM_NAME  Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(CMAKE_CROSSCOMPILING 1)

# Set toolchain paths
set(TOOLCHAIN arm-none-eabi)

#set(TOOLCHAIN_BIN_DIR "${TOOLCHAIN_PREFIX}/bin")
#set(TOOLCHAIN_INC_DIR "${TOOLCHAIN_PREFIX}/${TOOLCHAIN}/include")
#set(TOOLCHAIN_LIB_DIR "${TOOLCHAIN_PREFIX}/${TOOLCHAIN}/lib")

# Set system depended extensions
if(WIN32)
  set(TOOLCHAIN_EXT ".exe" )
else()
  set(TOOLCHAIN_EXT "" )
endif()

# Perform compiler test with static library
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(NRF_FLAGS "")

set(OBJECT_GEN_FLAGS "-mthumb -mabi=aapcs -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16")
# Set compiler linker flags
set(OBJECT_CXX_GEN_GLAGS "${OBJECT_GEN_FLAGS} -fdata-sections -ffunction-sections -fno-builtin -fno-strict-aliasing -fshort-enums -Wall -Werror")

set(CMAKE_C_FLAGS   "${OBJECT_CXX_GEN_GLAGS} -std=c99" CACHE INTERNAL "C Compiler options")
set(CMAKE_CXX_FLAGS "${OBJECT_CXX_GEN_GLAGS} -fno-exceptions -fno-rtti -Wno-register -std=c++17 " CACHE INTERNAL "C++ Compiler options")
set(CMAKE_ASM_FLAGS "${OBJECT_GEN_FLAGS} -g3" CACHE INTERNAL "ASM Compiler options")

set(CMAKE_EXE_LINKER_FLAGS "${OBJECT_GEN_FLAGS} -Wl,--gc-sections --specs=nano.specs --specs=nosys.specs -lc -lnosys -lm" CACHE INTERNAL "Linker options")

# Debug/Release configuration
set(CMAKE_C_FLAGS_DEBUG "-Og -g3 -DDEBUG" CACHE INTERNAL "C Compiler options for debug build type")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g3 -DDEBUG" CACHE INTERNAL "C++ Compiler options for debug build type")
set(CMAKE_ASM_FLAGS_DEBUG "" CACHE INTERNAL "ASM Compiler options for debug build type")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "" CACHE INTERNAL "Linker options for debug build type")

set(CMAKE_C_FLAGS_RELEASE "-O2 -flto" CACHE INTERNAL "C Compiler options for release build type")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -flto" CACHE INTERNAL "C++ Compiler options for release build type")
set(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "ASM Compiler options for release build type")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto" CACHE INTERNAL "Linker options for release build type")

set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g3 -flto" CACHE INTERNAL "C Compiler options for release + dbg info build type")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g3 -flto" CACHE INTERNAL "C++ Compiler options for release + dbg info build type")
set(CMAKE_ASM_FLAGS_RELWITHDEBINFO "" CACHE INTERNAL "ASM Compiler options for release + dbg info build type")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "-flto" CACHE INTERNAL "Linker options for release + dbg info build type")

set(CMAKE_C_FLAGS_MINSIZEREL "-Os -flto" CACHE INTERNAL "C Compiler options for minsize release build type")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -flto" CACHE INTERNAL "C++ Compiler options for minsize release build type")
set(CMAKE_ASM_FLAGS_MINSIZEREL "" CACHE INTERNAL "ASM Compiler options for minsize release build type")
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "-flto" CACHE INTERNAL "Linker options for minsize release build type")

# Link configuration
#set(CMAKE_EXE_LINKER_FLAGS "-T\"${NRF5_SDK_PATH}/modules/nrfx/mdk/nrf52840_xxaa.ld\"")

# Setup compilers
#set(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc${TOOLCHAIN_EXT} CACHE INTERNAL "C Compiler")
#set(CMAKE_CXX_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-g++${TOOLCHAIN_EXT} CACHE INTERNAL "C++ Compiler")
#set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc${TOOLCHAIN_EXT} CACHE INTERNAL "ASM Compiler")
#set(CMAKE_OBJCOPY_BIN ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-objcopy${TOOLCHAIN_EXT} CACHE INTERNAL "objcopy executable")
#set(CMAKE_SIZE_BIN ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-size${TOOLCHAIN_EXT} CACHE INTERNAL "size executable")
set(CMAKE_C_COMPILER arm-none-eabi-gcc${TOOLCHAIN_EXT} CACHE INTERNAL "C Compiler")
set(CMAKE_CXX_COMPILER arm-none-eabi-g++${TOOLCHAIN_EXT} CACHE INTERNAL "C++ Compiler")
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc${TOOLCHAIN_EXT} CACHE INTERNAL "ASM Compiler")
set(CMAKE_OBJCOPY_BIN arm-none-eabi-objcopy${TOOLCHAIN_EXT} CACHE INTERNAL "objcopy executable")
set(CMAKE_SIZE_BIN arm-none-eabi-size${TOOLCHAIN_EXT} CACHE INTERNAL "size executable")

set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_PREFIX}/${${TOOLCHAIN}} ${CMAKE_PREFIX_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)