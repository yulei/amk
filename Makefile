PROJECT_NAME     := amk
#TARGETS          := nrf52832_xxaa
OUTPUT_DIRECTORY := _build

TOP_DIR ?= .

TARGET := $(filter-out clean flash flash_softdevice sdk_config help, $(MAKECMDGOALS))

NRF5SDK_DIR := $(TOP_DIR)/nrf5_sdk/nRF5_SDK_17.0.0_9d13099

$(OUTPUT_DIRECTORY)/$(TARGET).out: \
  LINKER_SCRIPT  := $(TOP_DIR)/nrf5_sdk/nrf52832.ld

include $(TOP_DIR)/nrf5_sdk/nrf5.mk
include $(TOP_DIR)/tinyusb/tinyusb.mk
include $(TOP_DIR)/tmk/tmk.mk
include $(TOP_DIR)/main/main.mk

ifneq (, $(TARGET))
	include $(TOP_DIR)/keyboards/$(TARGET)/$(TARGET).mk
endif

APP_DEFS += $(NRF5_DEFS)
APP_DEFS += $(TMK_DEFS)
APP_DEFS += $(MAIN_DEFS)

# Libraries common to all targets
LIB_FILES += \

# Optimization flags
OPT = -O3 -g3
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
LDFLAGS += -mthumb -mabi=aapcs -L$(NRF5SDK_DIR)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

$(TARGET): CFLAGS += -D__HEAP_SIZE=8192
$(TARGET): CFLAGS += -D__STACK_SIZE=8192
$(TARGET): ASMFLAGS += -D__HEAP_SIZE=8192
$(TARGET): ASMFLAGS += -D__STACK_SIZE=8192

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm

.PHONY: default help

# Default target - first one defined
default: $(TARGET)

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		keyboard
	@echo		flash_softdevice
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

#TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc
include $(TOP_DIR)/common.mk
#$(foreach target, $(TARGETS), $(call define_target, $(target)))
$(call define_target, $(TARGET))


.PHONY: flash flash_softdevice erase

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
