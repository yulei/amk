
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
	$(MAIN_DIR)/usb/udd_desc_stub.c \

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
	INCS += $(LIB_DIR)/cmsis_5/CMSIS/RTOS2/Include
#	SRCS += $(MAIN_DIR)/rtos/freertos_main.c
	SRCS += $(MAIN_DIR)/rtos/rtos_main.c
#	include $(LIB_DIR)/cmsis_os.mk
	include $(LIB_DIR)/cmsis_freertos.mk
	APP_DEFS += -DRTOS_ENABLE
else
	ifeq (yes, $(strip $(LPM_ENABLE)))
		SRCS += $(MAIN_DIR)/lpm/lpm_main.c
		SRCS += $(MAIN_DIR)/lpm/stm32l4xx_lpm.c
		APP_DEFS += -DLPM_ENABLE
	else
		SRCS += $(MAIN_DIR)/amk/baremetal_main.c
	endif
endif

ifeq (yes, $(strip $(USB_DEVICE_ENABLE)))
	SRCS += $(MAIN_DIR)/usb/usb_descriptors.c
	APP_DEFS += -DUSB_DEVICE_ENABLE
endif