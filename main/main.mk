
MAIN_DIR := $(TOP_DIR)/main

SRC_FILES += \
	$(MAIN_DIR)/main.c  \
	$(MAIN_DIR)/matrix_driver.c \
	$(MAIN_DIR)/xprintf.c \
	$(MAIN_DIR)/porting/nrf52/timer.c \
	$(MAIN_DIR)/porting/nrf52/bootloader.c \
	$(MAIN_DIR)/porting/nrf52/gpio_pin.c \
	$(MAIN_DIR)/porting/nrf52/wait.c \
	$(MAIN_DIR)/porting/nrf52/eeconfig_fds.c \
	$(MAIN_DIR)/porting/nrf52/ble/ble_adv_service.c \
	$(MAIN_DIR)/porting/nrf52/ble/ble_bas_service.c \
	$(MAIN_DIR)/porting/nrf52/ble/ble_hids_service.c \
	$(MAIN_DIR)/porting/nrf52/ble/ble_services.c \
	$(MAIN_DIR)/porting/nrf52/nrf52832.c \
	$(MAIN_DIR)/porting/nrf52/ble_keyboard.c \
	$(MAIN_DIR)/porting/nrf52/usb_backend_uart.c \
	$(MAIN_DIR)/drivers/aw9523b.c \
	$(MAIN_DIR)/drivers/i2c.c \
	$(MAIN_DIR)/rgb_effects.c \
	$(MAIN_DIR)/rgb_driver.c \
	$(MAIN_DIR)/rgb_color.c \
	$(MAIN_DIR)/custom_action.c \
	$(MAIN_DIR)/eeprom_manager.c \
	#MAIN_DIR)/suspend.c \

INC_FOLDERS += \
	$(MAIN_DIR) \
	$(MAIN_DIR)/porting \
	$(MAIN_DIR)/porting/nrf52 \
	$(MAIN_DIR)/porting/nrf52/ble \
	$(MAIN_DIR)/drivers \

MAIN_DEFS += -DMCU_NRF5X
MAIN_DEFS += -DMATRIX_USE_GPIO