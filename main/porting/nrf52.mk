
NRF52_PORTING_DIR := $(TOP_DIR)/main/porting/nrf52

SRC_FILES += \
	$(NRF52_PORTING_DIR)/timer.c \
	$(NRF52_PORTING_DIR)/bootloader.c \
	$(NRF52_PORTING_DIR)/gpio_pin.c \
	$(NRF52_PORTING_DIR)/wait.c \
	$(NRF52_PORTING_DIR)/ble/ble_adv_service.c \
	$(NRF52_PORTING_DIR)/ble/ble_bas_service.c \
	$(NRF52_PORTING_DIR)/ble/ble_hids_service.c \
	$(NRF52_PORTING_DIR)/ble/ble_services.c \
	$(NRF52_PORTING_DIR)/ble_keyboard.c \
	$(NRF52_PORTING_DIR)/usb_backend_uart.c \

INC_FOLDERS += \
	$(NRF52_PORTING_DIR) \
	$(NRF52_PORTING_DIR)/ble \

ifeq (NRF52832, $(strip $(MCU)))
	SRC_FILES += $(MAIN_DIR)/porting/nrf52/nrf52832.c
endif