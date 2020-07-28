
set(TMK_PATH "tmk/tmk_core")

add_library(tmk STATIC "")

target_sources(tmk
    PRIVATE 
        ${TMK_PATH}/common/host.c
        ${TMK_PATH}/common/keyboard.c
        ${TMK_PATH}/common/matrix.c
        ${TMK_PATH}/common/action.c
        ${TMK_PATH}/common/action_tapping.c 
        ${TMK_PATH}/common/action_macro.c
        ${TMK_PATH}/common/action_layer.c
        ${TMK_PATH}/common/action_util.c 
        ${TMK_PATH}/common/print.c 
        ${TMK_PATH}/common/debug.c
        ${TMK_PATH}/common/util.c 
        ${TMK_PATH}/common/hook.c
        ${TMK_PATH}/common/keymap.c
        ${TMK_PATH}/common/bootmagic.c
        ${TMK_PATH}/common/mousekey.c
    ) 

target_include_directories(tmk
    PRIVATE
        ${CONFIG_INCLUDE_PATH}
    PUBLIC
        ${TMK_PATH}/common
        ${TMK_PATH}/
        ${CMAKE_CURRENT_LIST_DIR})

message(STATUS ${CONFIG_INCLUDE_PATH})

target_compile_definitions(tmk
    PUBLIC
        BOOTMAGIC_ENABLE
        MOUSEKEY_ENABLE
        EXTRAKEY_ENABLE
    )

target_compile_options(tmk
    PUBLIC   
        -include config.h
    )