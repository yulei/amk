
# printf
target_sources(${KEYBOARD} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/printf/printf.c)
target_include_directories(${KEYBOARD} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/printf ${CMAKE_CURRENT_LIST_DIR}/config)
target_compile_definitions(${KEYBOARD} PRIVATE -DPRINTF_INCLUDE_CONFIG_H)

# tmk
target_sources(${KEYBOARD}
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/tmk/tmk_core/common/host.c
	${CMAKE_CURRENT_LIST_DIR}/tmk/tmk_core/common/keyboard.c
	${CMAKE_CURRENT_LIST_DIR}/tmk/tmk_core/common/matrix.c
	${CMAKE_CURRENT_LIST_DIR}/tmk/tmk_core/common/action.c
	${CMAKE_CURRENT_LIST_DIR}/tmk/tmk_core/common/action_tapping.c
	${CMAKE_CURRENT_LIST_DIR}/tmk/tmk_core/common/action_macro.c
	${CMAKE_CURRENT_LIST_DIR}/tmk/tmk_core/common/action_layer.c
	${CMAKE_CURRENT_LIST_DIR}/tmk/tmk_core/common/action_util.c
	${CMAKE_CURRENT_LIST_DIR}/tmk/tmk_core/common/debug.c
	${CMAKE_CURRENT_LIST_DIR}/tmk/tmk_core/common/util.c
	${CMAKE_CURRENT_LIST_DIR}/tmk/tmk_core/common/hook.c
	)

target_include_directories(${KEYBOARD}
	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/tmk/tmk_core/common
	${CMAKE_CURRENT_LIST_DIR}/config
	)

if(MOUSEKEY_ENABLE)
	target_sources(${KEYBOARD}
		PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/tmk/tmk_core/common/mousekey.c
	)
	target_compile_definitions(${KEYBOARD} PUBLIC "MOUSEKEY_ENABLE")
	target_compile_definitions(${KEYBOARD} PUBLIC "MOUSE_ENABLE")
endif()

if(EXTRAKEY_ENABLE)
	target_compile_definitions(${KEYBOARD} PUBLIC "EXTRAKEY_ENABLE")
endif()

if(ACTIONMAP_ENABLE)
	target_sources(${KEYBOARD}
		PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/tmk/tmk_core/common/actionmap.c
	)
	target_compile_definitions(${KEYBOARD} PUBLIC "ACTIONMAP_ENABLE")
else()
	target_sources(${KEYBOARD}
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/tmk/tmk_core/common/keymap.c
	)
endif()

target_compile_options(${KEYBOARD} PUBLIC -include ${KEYBOARD_DIR}/config.h)

# tinyusb
if(TINYUSB_ENABLE)
    target_sources(${KEYBOARD}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/tinyusb/src/tusb.c
        ${CMAKE_CURRENT_LIST_DIR}/tinyusb/src/class/hid/hid_device.c
        ${CMAKE_CURRENT_LIST_DIR}/tinyusb/src/common/tusb_fifo.c
        ${CMAKE_CURRENT_LIST_DIR}/tinyusb/src/device/usbd.c
        ${CMAKE_CURRENT_LIST_DIR}/tinyusb/src/device/usbd_control.c
        )
    
    target_include_directories(${KEYBOARD}
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/tinyusb/src
		${CMAKE_CURRENT_LIST_DIR}/config
        )
    
    if(TINYUSB_USE_HAL)
		if(MCU_FAMILY MATCHES "stm32")
			target_sources(${KEYBOARD}
				PRIVATE
				${CMAKE_CURRENT_LIST_DIR}/../sdk_stm32/hal_usb/dcd_hal_stm32.c
				)
		endif()
    else()
        message(STATUS "tinyusb use hal not defined")
    endif()

    if(MSC_ENABLE)
        target_sources(${KEYBOARD}
            PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/tinyusb/src/class/msc/msc_device.c
            )
    endif()

    target_compile_definitions(${KEYBOARD} PUBLIC "TINYUSB_ENABLE")
endif()

# fatfs
if(MSC_ENABLE)
	target_sources(${KEYBOARD}
		PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/fatfs/source/ff.c
		${CMAKE_CURRENT_LIST_DIR}/fatfs/porting/diskio_flash.c
		)

	target_include_directories(${KEYBOARD}
		PUBLIC
		${CMAKE_CURRENT_LIST_DIR}/fatfs/source
		${CMAKE_CURRENT_LIST_DIR}/fatfs/porting
		)
endif()