
SRCS += \
	$(MAIN_DIR)/amk/main.c  \
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
	$(MAIN_DIR)/rtt \
	$(LIB_DIR)/tinyusb/src \


include $(MAIN_DIR)/feature.mk

ifneq (,$(filter $(strip $(MCU)),$(NRF_MCUS)))
	include $(MAIN_DIR)/porting/nrf52.mk
else
	include $(MAIN_DIR)/porting/wired.mk
endif
