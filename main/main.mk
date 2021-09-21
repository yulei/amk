
SRCS += \
	$(MAIN_DIR)/amk/amk_action.c \
	$(MAIN_DIR)/amk/amk_keymap.c \
	$(MAIN_DIR)/amk/amk_macro.c \
	$(MAIN_DIR)/amk/amk_printf.c \
	$(MAIN_DIR)/amk/amk_eeprom.c \
	$(MAIN_DIR)/amk/amk_boot.c \
	$(MAIN_DIR)/amk/report_queue.c \
	$(MAIN_DIR)/usb/usb_descriptors.c \

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
#	SRCS += $(MAIN_DIR)/rtos/cmos_main.c
#	SRCS += $(MAIN_DIR)/rtos/cmos_usb.c
#	include $(LIB_DIR)/freertos.mk
	APP_DEFS += -DRTOS_ENABLE
else
	SRCS += $(MAIN_DIR)/amk/main.c
endif