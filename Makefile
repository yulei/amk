
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

GOALS := $(filter-out clean flash erase flash_softdevice sdk_config, $(MAKECMDGOALS))

OUTPUT_DIRECTORY := build

ifneq (, $(GOALS))
GOAL_LIST := $(subst /, ,$(GOALS))
TARGET := $(lastword $(GOAL_LIST))
ifeq ($(words $(GOAL_LIST)),1)
BASE_DIR := keyboards
else ifeq ($(words $(GOAL_LIST)),2)
BASE_DIR := keyboards/$(firstword $(GOAL_LIST))
$(GOALS): $(TARGET)
else
$(error Invalid target: $(GOALS))
endif

KEYBOARDS := $(sort $(notdir $(wildcard $(BASE_DIR)/*)))
KEYBOARD_DIR := $(BASE_DIR)/$(TARGET)
ifneq (,$(filter $(TARGET),$(KEYBOARDS)))
include $(KEYBOARD_DIR)/$(TARGET).mk
else
$(error Unsupported Target: $(GOALS))
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

.PHONY: default list flash erase

# Default target
ifeq (,$(TARGET))
default: list 
endif

list:
	$(info Available Keyboards:)
	$(foreach kbd,$(sort $(notdir $(wildcard keyboards/*))),$(info -- $(kbd)))
	@echo

include common.mk

ifneq (,$(filter $(strip $(MCU)),$(NRF_MCUS)))
.PHONY: flash_softdevice

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/$(TARGET).hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/$(TARGET).hex --sectorerase
	nrfjprog -f nrf52 --reset

# Flash softdevice
ifeq (NRF52832, $(strip $(MCU)))
SOFTDEVICE_FILE := $(NRF5SDK_DIR)/components/softdevice/s132/hex/s132_nrf52_7.2.0_softdevice.hex
else
SOFTDEVICE_FILE := $(NRF5SDK_DIR)/components/softdevice/s140/hex/s140_nrf52_7.2.0_softdevice.hex
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

endif #NRF MCUS


FLASH_TARGET := $(filter $(strip $(MCU)),$(STM32_MCUS))
ifneq (,$(FLASH_TARGET))
ifeq (STM32F411, $(FLASH_TARGET))
FLASH_TARGET := STM32F411xE
endif
ifeq (STM32F405, $(FLASH_TARGET))
FLASH_TARGET := STM32F405xG
endif
ifeq (STM32F722, $(FLASH_TARGET))
FLASH_TARGET := STM32F722xE
endif
ifeq (STM32F103, $(FLASH_TARGET))
FLASH_TARGET := STM32F103xB
endif

flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/$(TARGET).hex
	pylink flash -t swd -d $(FLASH_TARGET) $(OUTPUT_DIRECTORY)/$(TARGET).hex

erase:
	pylink erase -t swd -d $(FLASH_TARGET) 

endif #STM32 MCUS
