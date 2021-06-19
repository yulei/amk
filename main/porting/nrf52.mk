
PLATFORM_PORTING_DIR := $(MAIN_DIR)/porting/nrf52

SRCS += \
	$(PLATFORM_PORTING_DIR)/generic_nrf52.c \
	$(PLATFORM_PORTING_DIR)/timer.c \
	$(PLATFORM_PORTING_DIR)/bootloader.c \
	$(PLATFORM_PORTING_DIR)/gpio_pin.c \
	$(PLATFORM_PORTING_DIR)/suspend.c \
	$(PLATFORM_PORTING_DIR)/wait.c \
	$(PLATFORM_PORTING_DIR)/ble/ble_adv_service.c \
	$(PLATFORM_PORTING_DIR)/ble/ble_bas_service.c \
	$(PLATFORM_PORTING_DIR)/ble/ble_hids_service.c \
	$(PLATFORM_PORTING_DIR)/ble/ble_services.c \
	$(PLATFORM_PORTING_DIR)/ble_keyboard.c \
	$(PLATFORM_PORTING_DIR)/gzll_keyboard.c \
	$(PLATFORM_PORTING_DIR)/rf_keyboard.c \
	$(PLATFORM_PORTING_DIR)/rf_power.c \

INCS += \
	$(PLATFORM_PORTING_DIR) \
	$(PLATFORM_PORTING_DIR)/ble \

ifeq (yes, $(strip JLINK_MMD))
	SRCS += $(PLATFORM_PORTING_DIR)/mmd/JLINK_MONITOR_ISR_SES.c $(PLATFORM_PORTING_DIR)/mmd/JLINK_MONITOR.c
	INCS += $(PLATFORM_PORTING_DIR)/mmd
	APP_DEFS += -DCONFIG_JLINK_MONITOR_ENABLED=1
endif

ifeq (NRF52832, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/usb_backend_uart.c
endif

ifeq (NRF52840, $(strip $(MCU)))
	SRCS += $(PLATFORM_PORTING_DIR)/usb_backend_tinyusb.c
#SRCS += $(NRF52_PORTING_DIR)/usb_backend_usb.c
endif
