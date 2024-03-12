
USB_DEVICE_ENABLE ?= yes
HID_OTHER_ENABLE ?= yes

ifneq (no,$(strip $(KEYBOARD_ENABLE)))
	APP_DEFS += -DKEYBOARD_ENABLE
	ifeq (yes, $(strip $(HID_OTHER_ENABLE)))
		MOUSEKEY_ENABLE = yes
		EXTRAKEY_ENABLE = yes
		APP_DEFS += -DHID_OTHER_ENABLE
	endif
endif

ifeq (yes, $(strip $(CDC_ENABLE)))
	APP_DEFS += -DCDC_ENABLE
endif

SRCS += \
	$(MAIN_DIR)/amk/amk_printf.c \
	$(MAIN_DIR)/amk/amk_indicator.c \
	$(MAIN_DIR)/amk/amk_cmd.c \
	$(MAIN_DIR)/amk/amk_profile.c \
	$(MAIN_DIR)/amk/amk_process_keycode.c \
	$(MAIN_DIR)/amk/amk_queue.c \
	$(MAIN_DIR)/amk/amk_eeprom.c \
	$(MAIN_DIR)/amk/ring_buffer.c \
	$(MAIN_DIR)/amk/linear_buffer.c \
	$(MAIN_DIR)/amk/amk_protocol.c \
	$(MAIN_DIR)/amk/amk_store.c \

INCS += \
	$(MAIN_DIR) \
	$(MAIN_DIR)/amk \
	$(MAIN_DIR)/porting \
	$(MAIN_DIR)/drivers \
	$(MAIN_DIR)/rgb \
	$(MAIN_DIR)/screen \
	$(MAIN_DIR)/usb \
	$(MAIN_DIR)/segger \
	$(LIB_DIR)/tinyusb/src \


include $(MAIN_DIR)/feature.mk

ifneq (,$(filter $(strip $(MCU)),$(NRF_MCUS)))
	include $(MAIN_DIR)/porting/nrf52.mk
else
	include $(MAIN_DIR)/porting/wired.mk
endif

ifeq (yes, $(strip $(RTOS_ENABLE)))
	INCS += $(MAIN_DIR)/rtos
#	SRCS += $(MAIN_DIR)/rtos/freertos_main.c
#	include $(LIB_DIR)/cmsis_os.mk
#	SRCS += $(MAIN_DIR)/rtos/rtos_main_v2.c
#	include $(LIB_DIR)/cmsis_freertos.mk
#	SRCS += $(MAIN_DIR)/rtos/rtos_main_rtx.c
#	include $(LIB_DIR)/cmsis_rtx.mk
	SRCS += $(MAIN_DIR)/rtos/tx_initialize_low_level.s
	SRCS += $(MAIN_DIR)/rtos/rtos_main_tx.c
	include $(LIB_DIR)/threadx.mk
	APP_DEFS += -DRTOS_ENABLE
	ifeq (yes, $(strip $(LPM_ENABLE)))
		APP_DEFS += -DLPM_ENABLE
	endif
else
	SRCS += $(MAIN_DIR)/amk/baremetal_main.c
endif

ifeq (yes, $(strip $(USB_DEVICE_ENABLE)))
	SRCS += $(MAIN_DIR)/usb/usb_descriptors.c
	APP_DEFS += -DUSB_DEVICE_ENABLE
endif

ifeq (, $(strip $(AMK_CUSTOM_MATRIX)))
	SRCS += $(MAIN_DIR)/amk/amk_mx_matrix.c
	ifeq (yes, $(strip $(PORT_SCAN_ENABLE)))
		APP_DEFS += -DPORT_SCAN_ENABLE
	endif
	ifeq (yes, $(strip $(STATE_SCAN_ENABLE)))
		SRCS += $(MAIN_DIR)/amk/amk_state.c
		APP_DEFS += -DSTATE_SCAN_ENABLE
	endif
endif

ifeq (yes, $(strip $(NOFRENDO_ENABLE)))
	SRCS += $(MAIN_DIR)/screen/nofrendo/emuapi.c
	SRCS += $(MAIN_DIR)/screen/nofrendo/libsnss.c
	SRCS += $(MAIN_DIR)/screen/nofrendo/nes_display.c
	SRCS += $(MAIN_DIR)/screen/nofrendo/nes_emu.c
	SRCS += $(MAIN_DIR)/screen/nofrendo/nofrendo_emu.c
	SRCS += $(MAIN_DIR)/screen/nofrendo/static_memory.c
	INCS += $(MAIN_DIR)/screen/nofrendo
endif