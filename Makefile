
PROJECT_NAME := amk
NRF_MCUS := NRF52832 NRF52840
STM32_MCUS := STM32F103 STM32F411 STM32F405 STM32F722 STM32L432 STM32L072 STM32F446 STM32F401 STM32G431 STM32L452 STM32F412
ATSAMD_MCUS := ATSAMD21
NUVOTON_MCUS := NUC126
GD32_MCUS := GD32E103 GD32E505
HC32_MCUS := HC32F460
M480_MCUS := M484

# Source files
SRCS += \

# Include folders
INCS += \

# Libraries
LIBS += \

# Definitions
APP_DEFS += \

GOALS := $(filter-out default list clean flash erase flash_softdevice sdk_config, $(MAKECMDGOALS))

OUTPUT_DIR := build
MAIN_DIR := main
LIB_DIR := libs
#NRF5_DIR := sdk_nrf5
#STM32_DIR := sdk_stm32
#ATSAMD_DIR := sdk_samd
#NUVOTON_DIR := sdk_nuvoton
#GD32_DIR := sdk_gd32

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
INCS += $(KEYBOARD_DIR)
INCS += $(LIB_DIR)/config

ifneq (,$(filter $(TARGET),$(KEYBOARDS)))
include $(wildcard $(KEYBOARD_DIR)/*.mk)
else
$(error Unsupported Target: $(GOALS))
endif

include $(MAIN_DIR)/main.mk
include $(LIB_DIR)/tmk.mk
include $(LIB_DIR)/printf.mk
ifneq (,$(filter $(strip $(MCU)),$(NRF_MCUS)))
VENDOR_DIR := libs/vendor/nordic
include sdk_nrf5/nrf5_sdk.mk
else ifneq (,$(filter $(strip $(MCU)),$(STM32_MCUS)))
VENDOR_DIR := libs/vendor/st
include sdk_stm32/stm32_sdk.mk
else ifneq (,$(filter $(strip $(MCU)),$(ATSAMD_MCUS)))
VENDOR_DIR := libs/vendor/microchip
include sdk_samd/atsamd_sdk.mk
else ifneq (,$(filter $(strip $(MCU)),$(GD32_MCUS)))
VENDOR_DIR := libs/vendor/gigadevice
include sdk_gd32/gd32_sdk.mk
else ifneq (,$(filter $(strip $(MCU)),$(HC32_MCUS)))
VENDOR_DIR := libs/vendor/hdsc
include sdk_hc32/hc32_sdk.mk
else ifneq (,$(filter $(strip $(MCU)),$(M480_MCUS)))
VENDOR_DIR := libs/vendor/nuvoton
include sdk_m480/m480_sdk.mk
else
$(error Unsupported MCU: $(MCU))
endif
endif

ifeq (yes,$(MSC_ENABLE))
include $(LIB_DIR)/fatfs.mk
endif

ifeq (yes, $(strip $(VIAL_ENABLE)))
$(MAIN_DIR)/vial/vial_porting.c: $(KEYBOARD_DIR)/vial_generated_keyboard_definition.h

$(KEYBOARD_DIR)/vial_generated_keyboard_definition.h: $(KEYBOARD_DIR)/vial.json
	python3 util/vial_generate_definition.py $(KEYBOARD_DIR)/vial.json $(KEYBOARD_DIR)/vial_generated_keyboard_definition.h

endif

.PHONY: default list clean flash erase

# Default target
ifeq (,$(TARGET))
default: list 
else
$(info Building $(TARGET) ...)
endif

clean:
	$(RM) $(OUTPUT_DIR)

list:
	$(info Available Keyboards:)
	$(foreach kbd,$(sort $(notdir $(wildcard keyboards/*))),$(info -- $(kbd)))
	@echo

include common.mk

ifneq (,$(filter $(strip $(MCU)),$(NRF_MCUS)))
.PHONY: flash_softdevice

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIR)/$(TARGET).hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIR)/$(TARGET).hex --sectorerase
	nrfjprog -f nrf52 --reset

# Flash softdevice
ifeq (NRF52832, $(strip $(MCU)))
SOFTDEVICE_TYPE = 132
else
SOFTDEVICE_TYPE = 140
endif
SOFTDEVICE_VERSION = 7.2.0 
SOFTDEVICE_FILE := $(NRF5SDK_DIR)/components/softdevice/$(SOFTDEVICE_TYPE)/hex/s132_nrf52_$(SOFTDEVICE_VERSION)_softdevice.hex

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
	@echo Flashing: $(OUTPUT_DIR)/$(TARGET).hex
	pylink flash -t swd -d $(FLASH_TARGET) $(OUTPUT_DIR)/$(TARGET).hex

erase:
	pylink erase -t swd -d $(FLASH_TARGET) 

endif #STM32 MCUS
