NRF52_PORTING_DIR := $(MAIN_DIR)/porting/nrf52

SRCS += \
	$(NRF52_PORTING_DIR)/generic_nrf52.c \
	$(NRF52_PORTING_DIR)/timer.c \
	$(NRF52_PORTING_DIR)/bootloader.c \
	$(NRF52_PORTING_DIR)/gpio_pin.c \
	$(NRF52_PORTING_DIR)/wait.c \
	$(NRF52_PORTING_DIR)/ble/ble_adv_service.c \
	$(NRF52_PORTING_DIR)/ble/ble_bas_service.c \
	$(NRF52_PORTING_DIR)/ble/ble_hids_service.c \
	$(NRF52_PORTING_DIR)/ble/ble_services.c \
	$(NRF52_PORTING_DIR)/ble_keyboard.c \
	$(NRF52_PORTING_DIR)/gzll_keyboard.c \
	$(NRF52_PORTING_DIR)/rf_keyboard.c \
	$(NRF52_PORTING_DIR)/eeconfig_fds.c \

INCS += \
	$(NRF52_PORTING_DIR) \
	$(NRF52_PORTING_DIR)/ble \

ifeq (yes, $(strip JLINK_MMD))
	SRCS += $(NRF52_PORTING_DIR)/mmd/JLINK_MONITOR_ISR_SES.c $(NRF52_PORTING_DIR)/mmd/JLINK_MONITOR.c
	INCS += $(NRF52_PORTING_DIR)/mmd
	APP_DEFS += -DCONFIG_JLINK_MONITOR_ENABLED=1
endif

ifeq (NRF52832, $(strip $(MCU)))
	SRCS += $(NRF52_PORTING_DIR)/usb_backend_uart.c
endif

ifeq (NRF52840, $(strip $(MCU)))
	SRCS += $(NRF52_PORTING_DIR)/usb_backend_tinyusb.c
endif