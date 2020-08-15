PROJECT_NAME     := amk
OUTPUT_DIRECTORY := _build

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

ifneq (, $(TARGET))
	include $(TOP_DIR)/keyboards/$(TARGET)/$(TARGET).mk
endif

#include $(TOP_DIR)/tinyusb/tinyusb.mk
include $(TOP_DIR)/main/main.mk
include $(TOP_DIR)/tmk/tmk.mk

ifeq (NRF52832, $(strip $(MCU)))
include $(TOP_DIR)/nrf5_sdk/nrf5_sdk.mk
endif

ifeq (STM32F411, $(strip $(MCU)))
include $(TOP_DIR)/stm32_sdk/stm32_sdk.mk
endif



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
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT)

# Assembler flags common to all targets
ASMFLAGS += $(APP_DEFS)
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(LINKER_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
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

ifeq (NRF52832, $(strip $(MCU)))
.PHONY: flash_softdevice

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/$(TARGET).hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/$(TARGET).hex --sectorerase
	nrfjprog -f nrf52 --reset

# Flash softdevice
flash_softdevice:
	@echo Flashing: s132_nrf52_7.0.1_softdevice.hex
	nrfjprog -f nrf52 --program $(NRF5SDK_DIR)/components/softdevice/s132/hex/s132_nrf52_7.0.1_softdevice.hex --sectorerase
	nrfjprog -f nrf52 --reset

erase:
	nrfjprog -f nrf52 --eraseall

SDK_CONFIG_FILE := ./nrf5_sdk/sdk_config.h
CMSIS_CONFIG_TOOL := $(NRF5SDK_DIR)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)

endif

ifeq (STM32F411, $(strip $(MCU)))

flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/$(TARGET).hex
	pylink flash -t swd -d STM32F411xE $(OUTPUT_DIRECTORY)/$(TARGET).hex

erase:
	pylink erase -t swd -d STM32F411xE

endif