
PROJECT_NAME := amk
NRF_MCUS := NRF52832 NRF52840
STM32_MCUS := STM32F103 STM32F411 STM32F405 STM32F722

TOP_DIR ?= .

# Source files
SRCS += \

# Include folders
INCS += \

# Libraries
LIBS += \

# Definitions
APP_DEFS += \

GOAL := $(filter-out clean flash erase flash_softdevice sdk_config, $(MAKECMDGOALS))

OUTPUT_DIRECTORY := build

ifneq (, $(GOAL))
GOAL_P := $(subst /, ,$(GOAL))
TARGET := $(lastword $(GOAL_P))
ifeq ($(words $(GOAL_P)),1)
BASE_DIR := keyboards
else ifeq ($(words $(GOAL_P)),2)
BASE_DIR := keyboards/$(firstword $(GOAL_P))
$(GOAL): $(TARGET)
else
$(error Invalid target: $(GOAL))
endif

KEYBOARDS := $(sort $(notdir $(wildcard $(BASE_DIR)/*)))
ifneq (,$(filter $(TARGET),$(KEYBOARDS)))
include $(BASE_DIR)/$(TARGET)/$(TARGET).mk
endif

include main/main.mk
include lib/tmk.mk
include lib/printf.mk
ifneq (,$(filter $(strip $(MCU)),$(NRF_MCUS)))
include nrf5_sdk/nrf5_sdk.mk
else ifneq (,$(filter $(strip $(MCU)),$(STM32_MCUS)))
include stm32_sdk/stm32_sdk.mk
else
$(error Unsupported MCU: $(MCU))
endif
endif

ifeq (yes,$(SCREEN_ENABLE))
include lib/lvgl.mk
endif

.PHONY: default list

# Default target
ifeq (,$(TARGET))
default: list 
endif

KEYBOARDS := $(sort $(filter-out dev,$(notdir $(wildcard keyboards/*))))
list:
	$(info Following keyboards are available:)
	$(foreach kbd,$(KEYBOARDS),$(info -- $(kbd)))
	@echo

include common.mk

.PHONY: flash erase

ifneq (,$(filter $(strip $(MCU)),$(NRF_MCUS)))
.PHONY: flash_softdevice

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/$(TARGET).hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/$(TARGET).hex --sectorerase
	nrfjprog -f nrf52 --reset

# Flash softdevice
ifeq (NRF52832, $(strip $(MCU)))
SOFTDEVICE_FILE := $(NRF5SDK_DIR)/components/softdevice/s132/hex/s132_nrf52_7.0.1_softdevice.hex
else
SOFTDEVICE_FILE := $(NRF5SDK_DIR)/components/softdevice/s140/hex/s140_nrf52_7.0.1_softdevice.hex
endif
flash_softdevice:
	@echo Flashing: $(notdir $(SOFTDEVICE_FILE))
	nrfjprog -f nrf52 --program $(SOFTDEVICE_FILE) --sectorerase
	nrfjprog -f nrf52 --reset

erase:
	nrfjprog -f nrf52 --eraseall

SDK_CONFIG_FILE := ./nrf5_sdk/sdk_config.h
CMSIS_CONFIG_TOOL := $(NRF5SDK_DIR)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)

endif


FLASH_TARGET := $(filter $(strip $(MCU)),$(STM32_MCUS))
ifneq (,$(FLASH_TARGET))
	ifeq (STM32F411, $(FLASH_TARGET))
	FLASH_TARGET := STM32F411xE
	endif
	ifeq (STM32F405, $(FLASH_TARGET))
	FLASH_TARGET := STM32F405xG
	endif
	ifeq (STM32F722, $(FLASH_TARGET))
	FLASH_TARGET := STM32F22xE
	endif
	ifeq (STM32F103, $(FLASH_TARGET))
	FLASH_TARGET := STM32F103xB
	endif

flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/$(TARGET).hex
	pylink flash -t swd -d $(FLASH_TARGET) $(OUTPUT_DIRECTORY)/$(TARGET).hex

erase:
	pylink erase -t swd -d $(FLASH_TARGET) 

endif
