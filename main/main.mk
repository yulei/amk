
USB_DEVICE_ENABLE ?= yes
HID_OTHER_ENABLE ?= yes

ifeq (yes, $(strip $(HID_OTHER_ENABLE)))
	MOUSEKEY_ENABLE = yes
	EXTRAKEY_ENABLE = yes
	APP_DEFS += -DHID_OTHER_ENABLE
endif

ifeq (yes, $(strip $(CDC_ENABLE)))
	APP_DEFS += -DCDC_ENABLE
endif

SRCS += \
	$(MAIN_DIR)/amk/amk_action.c \
	$(MAIN_DIR)/amk/amk_keymap.c \
	$(MAIN_DIR)/amk/amk_macro.c \
	$(MAIN_DIR)/amk/amk_printf.c \
	$(MAIN_DIR)/amk/amk_eeprom.c \
	$(MAIN_DIR)/amk/amk_boot.c \
	$(MAIN_DIR)/amk/amk_indicator.c \
	$(MAIN_DIR)/amk/amk_cmd.c \
	$(MAIN_DIR)/amk/amk_queue.c \
	$(MAIN_DIR)/amk/amk_profile.c \
	$(MAIN_DIR)/amk/report_queue.c \
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
	SRCS += $(MAIN_DIR)/rtos/usbx_main.c
	SRCS += $(MAIN_DIR)/rtos/tx_initialize_low_level.S
	SRCS += $(MAIN_DIR)/rtos/usbx_usb.c
	SRCS += $(MAIN_DIR)/rtos/usbx_desc.c
	include $(LIB_DIR)/threadx.mk
	include $(LIB_DIR)/usbx.mk
	APP_DEFS += -DRTOS_ENABLE
else
	SRCS += $(MAIN_DIR)/amk/main.c
endif

ifeq (yes, $(strip $(USB_DEVICE_ENABLE)))
	SRCS += $(MAIN_DIR)/usb/usb_descriptors.c
	APP_DEFS += -DUSB_DEVICE_ENABLE
endif