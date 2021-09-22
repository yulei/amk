

if(MATRIX_USE_TCA6424)
    target_compile_definitions(${KEYBOARD} PRIVATE "MATRIX_I2C_PINS")
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/drivers/tca6424.c
        ${CMAKE_CURRENT_LIST_DIR}/drivers/i2c.c
        )
endif()

if(MATRIX_USE_PCA9535)
    target_compile_definitions(${KEYBOARD} PRIVATE "MATRIX_I2C_PINS")
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/drivers/pca9535.c
        ${CMAKE_CURRENT_LIST_DIR}/drivers/i2c.c
        )
endif()

if(SCREEN_ENABLE)
    target_compile_definitions(${KEYBOARD} PRIVATE "SCREEN_ENABLE")
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/drivers/spi.c
        ${CMAKE_CURRENT_LIST_DIR}/drivers/st7735.c
        ${CMAKE_CURRENT_LIST_DIR}/screen/screen.c
        )
endif()

if(RGB_LINEAR_ENABLE)
    target_compile_definitions(${KEYBOARD} PRIVATE "RGB_LINEAR_ENABLE")
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/rgb/rgb_linear.c
        )
    set(RGB_ENABLE yes)
endif()


if(RGB_INDICATOR_ENABLE)
    target_compile_definitions(${KEYBOARD} PRIVATE "RGB_INDICATOR_ENABLE")
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/rgb/rgb_indicator.c
        )
    set(RGB_ENABLE yes)
endif()


if(RGB_MATRIX_ENABLE)
    target_compile_definitions(${KEYBOARD} PRIVATE "RGB_MATRIX_ENABLE")
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/rgb/rgb_matrix.c
        ${CMAKE_CURRENT_LIST_DIR}/rgb/rgb_effect_matrix.c
        )
    set(RGB_ENABLE yes)
endif()

if(RGB_ENABLE)
    target_compile_definitions(${KEYBOARD} PRIVATE "RGB_ENABLE")
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/drivers/aw9106b.c
        ${CMAKE_CURRENT_LIST_DIR}/drivers/aw9523b.c
        ${CMAKE_CURRENT_LIST_DIR}/drivers/is31fl3236.c
        ${CMAKE_CURRENT_LIST_DIR}/drivers/is31fl3731.c
        ${CMAKE_CURRENT_LIST_DIR}/drivers/is31fl3733.c
        ${CMAKE_CURRENT_LIST_DIR}/drivers/ws2812.c
        ${CMAKE_CURRENT_LIST_DIR}/drivers/i2c.c
        ${CMAKE_CURRENT_LIST_DIR}/drivers/rgb_driver.c
        ${CMAKE_CURRENT_LIST_DIR}/rgb/rgb_common.c
        ${CMAKE_CURRENT_LIST_DIR}/rgb/rgb_led.c
        ${CMAKE_CURRENT_LIST_DIR}/rgb/rgb_effect_linear.c
        )
endif()

if(MSC_ENABLE)
    target_compile_definitions(${KEYBOARD} PRIVATE "MSC_ENABLE")
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/drivers/w25qxx.c
        ${CMAKE_CURRENT_LIST_DIR}/drivers/spi.c
        ${CMAKE_CURRENT_LIST_DIR}/screen/anim.c
        )
    if(RTOS_ENABLE)
        target_sources(${KEYBOARD}
            PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/rtos/usbx_msc.c
        )
    else()
        target_sources(${KEYBOARD}
            PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/usb/mscusb.c
        )
    endif()
endif()

if(VIAL_ENABLE)
    target_compile_definitions(${KEYBOARD} PRIVATE "VIAL_ENABLE")
    target_compile_definitions(${KEYBOARD} PRIVATE "KEYMAP_CONFIG_ENABLE")
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/vial/vial_porting.c
        ${CMAKE_CURRENT_LIST_DIR}/vial/vial_macro.c
        ${CMAKE_CURRENT_LIST_DIR}/vial/keycode_convert.c
        ${CMAKE_CURRENT_LIST_DIR}/vial/send_string.c
        )
    target_include_directories(${KEYBOARD} 
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/vial
        )
    set(ACTIONMAP_ENABLE yes)
endif()

if(NOT CUSTOM_MATRIX)
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/amk/matrix_scan.c
        )
endif()

if(EECONFIG_FRAM)
    target_compile_definitions(${KEYBOARD} PRIVATE "EECONFIG_FRAM")
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/amk/eeconfig_fram.c
        ${CMAKE_CURRENT_LIST_DIR}/drivers/mb85rcxx.c
        ${CMAKE_CURRENT_LIST_DIR}/drivers/i2c.c
        )
elseif(EECONFIG_FLASH)
    target_compile_definitions(${KEYBOARD} PRIVATE "EECONFIG_FLASH")
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/porting/wired/stm32/eeconfig_flash.c
        )
else()
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/amk/eeconfig_mem.c
        )
endif()

if(DATETIME_ENABLE)
    target_compile_definitions(${KEYBOARD} PRIVATE "DATETIME_ENABLE")
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/drivers/i2c.c
        )
    if(RTC_DRIVER STREQUAL "bl5372")
        target_sources(${KEYBOARD}
            PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/drivers/bl5372.c
            )
    else()
        target_sources(${KEYBOARD}
            PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/drivers/rtc8563.c
            )
    endif()
endif()

if(DYNAMIC_CONFIGURATION)
    target_compile_definitions(${KEYBOARD} PRIVATE "DYNAMIC_CONFIGURATION")
endif()
