
SRCS += \
	$(MAIN_DIR)/common/main.c  \
	$(MAIN_DIR)/common/amk_action.c \
	$(MAIN_DIR)/common/amk_keymap.c \
	$(MAIN_DIR)/common/amk_printf.c \
	$(MAIN_DIR)/common/amk_eeprom.c \
	$(MAIN_DIR)/common/amk_boot.c \
	$(MAIN_DIR)/common/report_queue.c \
	$(MAIN_DIR)/rgb/rgb_led.c \
	$(MAIN_DIR)/usb/usb_descriptors.c \

INCS += \
	$(MAIN_DIR) \
	$(MAIN_DIR)/common \
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
endif

ifneq (,$(filter $(strip $(MCU)),$(STM32_MCUS)))
	include $(MAIN_DIR)/porting/stm32.mk
endif

ifneq (,$(filter $(strip $(MCU)),$(ATSAMD_MCUS)))
	include $(MAIN_DIR)/porting/atsamd.mk
endif

ifneq (,$(filter $(strip $(MCU)),$(NUVOTON_MCUS)))
	include $(MAIN_DIR)/porting/nuvoton.mk
endif

ifneq (,$(filter $(strip $(MCU)),$(GD32_MCUS)))
	include $(MAIN_DIR)/porting/gd32.mk
endif
