STM32_PORTING_DIR := $(TOP_DIR)/main/porting/stm32

SRC_FILES += \
	$(STM32_PORTING_DIR)/timer.c \
	$(STM32_PORTING_DIR)/bootloader.c \
	$(STM32_PORTING_DIR)/wait.c \
	$(STM32_PORTING_DIR)/generic_stm32f4.c \
	$(STM32_PORTING_DIR)/usb_descriptors.c \
	$(STM32_PORTING_DIR)/gpio_pin.c \

INC_FOLDERS += \
	$(STM32_PORTING_DIR) \
