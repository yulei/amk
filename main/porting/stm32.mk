STM32_PORTING_DIR := $(TOP_DIR)/main/porting/stm32

SRC_FILES += \
	$(STM32_PORTING_DIR)/timer.c \
	$(STM32_PORTING_DIR)/bootloader.c \
	$(STM32_PORTING_DIR)/wait.c \
	$(STM32_PORTING_DIR)/generic_stm32.c \
	$(STM32_PORTING_DIR)/usb_descriptors.c \
	$(STM32_PORTING_DIR)/gpio_pin.c \

INC_FOLDERS += \
	$(STM32_PORTING_DIR) \

ifeq (STM32F411, $(strip $(MCU)))
	SRC_FILES += $(STM32_PORTING_DIR)/stm32f411xe.c
endif

ifeq (STM32L433, $(strip $(MCU)))
	SRC_FILES += $(STM32_PORTING_DIR)/stm32l433xx.c
endif

ifeq (STM32F722, $(strip $(MCU)))
	SRC_FILES += $(STM32_PORTING_DIR)/stm32f722xx.c
endif

ifeq (STM32F103, $(strip $(MCU)))
	SRC_FILES += $(STM32_PORTING_DIR)/stm32f103xb.c
endif