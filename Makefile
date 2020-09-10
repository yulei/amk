PROJECT_NAME     := amk

TOP_DIR ?= .

# Source files
SRC_FILES += \

# Include folders
INC_FOLDERS += \

# Libraries
LIB_FILES += \

# Definitions
APP_DEFS += \

TARGET := $(filter-out clean flash erase flash_softdevice sdk_config help, $(MAKECMDGOALS))

OUTPUT_DIRECTORY := build/$(TARGET)

ifneq (, $(TARGET))
include $(TOP_DIR)/keyboards/$(TARGET)/$(TARGET).mk
ifeq (NRF52832, $(strip $(MCU)))
include $(TOP_DIR)/nrf5_sdk/nrf5_sdk.mk
else ifeq (NRF52840, $(strip $(MCU)))
include $(TOP_DIR)/nrf5_sdk/nrf5_sdk.mk
else ifeq (STM32F411, $(strip $(MCU)))
include $(TOP_DIR)/stm32_sdk/stm32_sdk.mk
else ifeq (STM32L433, $(strip $(MCU)))
include $(TOP_DIR)/stm32_sdk/stm32_sdk.mk
else ifeq (STM32F722, $(strip $(MCU)))
include $(TOP_DIR)/stm32_sdk/stm32_sdk.mk
else
$(error Unsupported MCU: $(MCU))
endif
endif

include $(TOP_DIR)/lvgl/lvgl.mk
include $(TOP_DIR)/main/main.mk
include $(TOP_DIR)/tmk/tmk.mk


#$(OUTPUT_DIRECTORY)/$(TARGET).out: $(LINKER_SCRIPT)

# Optimization flags
#OPT = -O3 -g3

# Debug flags
OPT = -Og -g3 -DDEBUG

# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += $(APP_DEFS)
CFLAGS += $(SDK_DEFS)
#CFLAGS += -mcpu=cortex-m4
#CFLAGS += -mthumb -mabi=aapcs
#CFLAGS += -mthumb
#CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
#CFLAGS += -mfloat-abi=hard
CFLAGS += -Wall -Werror
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT)

# Assembler flags common to all targets
ASMFLAGS += $(OPT)
ASMFLAGS += $(APP_DEFS)
ASMFLAGS += $(SDK_DEFS)

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += $(SDK_DEFS)
LDFLAGS += -L$(LINKER_PATH) -T$(LINKER_SCRIPT)
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm

.PHONY: default help

# Default target - first one defined
ifneq (,$(TARGET))
	default: $(TARGET)
else
	default: help
endif

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		keyboard 			- your keyboard name
#	@echo		sdk_config 			- starting external tool for editing sdk_config.h
	@echo		flash      			- flashing binary
#	@echo		flash_softdevice	- flashing the softdevice
	@echo		erase				- erase the chip

#TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc
include $(TOP_DIR)/common.mk
#$(foreach target, $(TARGETS), $(call define_target, $(target)))
$(call define_target, $(TARGET))


.PHONY: flash erase

ifneq (,$(filter $(strip $(MCU)),NRF52832 NRF52840))
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


FLASH_TARGET := $(filter $(strip $(MCU)),STM32F411 STM32F722 STM32L433)
ifneq (,$(FLASH_TARGET))
	ifeq (STM32F411, $(FLASH_TARGET))
	FLASH_TARGET := STM32F411xE
	endif
	ifeq (STM32F722, $(FLASH_TARGET))
	FLASH_TARGET := STM32F22xE
	endif
	ifeq (STM32L433, $(FLASH_TARGET))
	FLASH_TARGET := STM32L433xC
	endif

flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/$(TARGET).hex
	pylink flash -t swd -d $(FLASH_TARGET) $(OUTPUT_DIRECTORY)/$(TARGET).hex

erase:
	pylink erase -t swd -d $(FLASH_TARGET) 

endif