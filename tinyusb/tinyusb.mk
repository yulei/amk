TINYUSB_DIR := $(TOP_DIR)/tinyusb

SRC_FILES += \

INC_FOLDERS += \
	$(TINYUSB_DIR) \
	$(TINYUSB_DIR)/tinyusb/src \

ifeq (NRF52832, $(strip $(MCU)))
APP_DEFS += \
	-DCFG_TUSB_MCU=OPT_MCU_NRF5X
endif

ifeq (STM32F411, $(strip $(MCU)))
APP_DEFS += \
	-DCFG_TUSB_MCU=OPT_MCU_STM32F411
endif
