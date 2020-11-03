
ifneq ($(words $(MAKECMDGOALS)),1) # if no argument was given to make...
.DEFAULT_GOAL = all # set the default goal to all
%:                   # define a last resort default rule
	@$(MAKE) $@ --no-print-directory -rRf $(firstword $(MAKEFILE_LIST)) # recursive make call, 
else
ifndef PROGRESS 
T := $(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory \
      -nrRf $(firstword $(MAKEFILE_LIST)) \
      PROGRESS="COUNTTHIS" | grep -c "COUNTTHIS")
N := x
C = $(words $N)$(eval N := x $N)
PROGRESS = echo "[`expr $C '*' 100 / $T`%]"
endif

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

TARGET := $(filter-out clean flash erase flash_softdevice sdk_config, $(MAKECMDGOALS))
KEYBOARDS := $(sort $(filter-out %.h %.c,$(notdir $(wildcard keyboards/*))))

OUTPUT_DIRECTORY := build

ifneq (, $(TARGET))
ifeq ($(filter $(TARGET),$(KEYBOARDS)),)
$(info A valid keyboard target must be provided, current available:)
$(foreach kbd,$(KEYBOARDS),$(info $(kbd)))
$(error Invalid Target specified)
endif
include keyboards/$(TARGET)/$(TARGET).mk
include main/main.mk
include lib/tmk.mk
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

# Optimization flags
#OPT = -O3 -g3

# Debug flags
OPT = -Og -g3 -DDEBUG

# Uncomment the line below to enable link time optimization
#OPT += -flto

FLAGS_ALL := $(OPT) $(APP_DEFS) $(SDK_DEFS)
# C flags common to all targets
CFLAGS += $(FLAGS_ALL)
CFLAGS += -Wall
CFLAGS += -Werror
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -fno-strict-aliasing
CFLAGS += -fno-builtin
CFLAGS += -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT)

# Assembler flags common to all targets
ASMFLAGS += $(FLAGS_ALL)

# Linker flags
LDFLAGS += $(FLAGS_ALL)
LDFLAGS += -L$(LINKER_PATH) -T$(LINKER_SCRIPT)
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIBS += -lc -lnosys -lm

.PHONY: default list

# Default target
ifneq (,$(TARGET))
	default: $(TARGET)
else
	default: list 
endif

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

endif