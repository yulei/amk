# Command alias
MK := mkdir -p
RM := rm -rf
CP := cp
UF2 := python util/uf2conv.py

# Build type
NO_DEBUG ?= 0
LTO_ENABLE ?= 0

# Echo suspend
NO_ECHO := @

GNU_PREFIX := arm-none-eabi

# Toolchain commands
CC      := $(GNU_INSTALL_ROOT)$(GNU_PREFIX)-gcc
CXX     := $(GNU_INSTALL_ROOT)$(GNU_PREFIX)-c++
AS      := $(GNU_INSTALL_ROOT)$(GNU_PREFIX)-as
AR      := $(GNU_INSTALL_ROOT)$(GNU_PREFIX)-ar -rcs
LD      := $(GNU_INSTALL_ROOT)$(GNU_PREFIX)-ld
NM      := $(GNU_INSTALL_ROOT)$(GNU_PREFIX)-nm
OBJDUMP := $(GNU_INSTALL_ROOT)$(GNU_PREFIX)-objdump
OBJCOPY := $(GNU_INSTALL_ROOT)$(GNU_PREFIX)-objcopy
SIZE    := $(GNU_INSTALL_ROOT)$(GNU_PREFIX)-size


BUILD_DIR := $(OUTPUT_DIR)/$(TARGET)

OBJS += $(patsubst %.s,$(BUILD_DIR)/%.o,$(patsubst %.S,$(BUILD_DIR)/%.o,$(filter %.s %.S,$(SRCS))))
OBJS += $(patsubst %.c,$(BUILD_DIR)/%.o,$(patsubst %.cpp,$(BUILD_DIR)/%.o,$(filter-out %.s %.S,$(SRCS))))

LIB_OBJS += $(patsubst %.s,$(BUILD_DIR)/%.o,$(patsubst %.S,$(BUILD_DIR)/%.o,$(filter %.s %.S,$(LIB_SRCS))))
LIB_OBJS += $(patsubst %.c,$(BUILD_DIR)/%.o,$(patsubst %.cpp,$(BUILD_DIR)/%.o,$(filter-out %.s %.S,$(LIB_SRCS))))

ifndef PROGRESS 
T := $(shell $(MAKE) $(GOALS) --no-print-directory \
      -nrRf $(firstword $(MAKEFILE_LIST)) \
      PROGRESS="COUNTTHIS" | grep -c "COUNTTHIS")
N := x
C = $(words $N)$(eval N := x $N)
PROGRESS = echo -n "[`expr $C '*' 100 / $T`%]"
endif

# Create build directories
OBJ_DIRS = $(sort $(dir $(OBJS)))
$(OBJS): | $(OBJ_DIRS)
$(OBJ_DIRS):
	$(NO_ECHO)$(MK) $@

LIB_OBJ_DIRS = $(sort $(dir $(LIB_OBJS)))
$(LIB_OBJS): | $(LIB_OBJ_DIRS)
$(LIB_OBJ_DIRS):
	$(NO_ECHO)$(MK) $@

#$(info $(SRCS))
#$(info $(INCS))
#$(info $(LIBS))
#$(info $(OBJS))
#$(info $(APP_DEFS))

ifeq (1,$(NO_DEBUG))
OPT = -Os -ggdb
else
OPT = -Og -ggdb3 -DDEBUG
endif

ifeq (1,$(LTO_ENABLE))
OPT += -flto
endif

COMMON_FLAGS := $(OPT) $(APP_DEFS) $(SDK_DEFS)
# C flags common to all targets
CFLAGS += $(COMMON_FLAGS)
CFLAGS += -Wall
CFLAGS += -Werror
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -fno-strict-aliasing
CFLAGS += -fno-builtin
CFLAGS += -fshort-enums
CFLAGS += -fms-extensions
# C++ flags common to all targets
CXXFLAGS += $(COMMON_FLAGS)

# Assembler flags common to all targets
ASMFLAGS += $(COMMON_FLAGS)

# Linker flags
LDFLAGS += $(COMMON_FLAGS)
LDFLAGS += -L$(LINKER_PATH) -T$(LINKER_SCRIPT)
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs
# let linker print memory usage
LDFLAGS += -Wl,--print-memory-usage 

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIBS += -lc -lnosys -lm

# Compiler flags to generate dependency files.
GENDEPFLAGS = -MMD 

INC_FLAGS = $(addprefix -I,$(sort $(strip $(INCS))))

ALL_CFLAGS = -std=gnu99 $(CFLAGS) $(GENDEPFLAGS) $(INC_FLAGS)
ALL_CPPFLAGS = -x c++ $(CPPFLAGS) $(GENDEPFLAGS) $(INC_FLAGS)
ALL_ASMFLAGS = -x assembler-with-cpp $(ASMFLAGS) $(GENDEPFLAGS) $(INC_FLAGS)
ALL_LDFLAGS = $(LDFLAGS) -Xlinker -Map=$(OUTPUT_DIR)/$(TARGET).map

PLATFORM := $(if $(filter Windows%,$(OS)),windows,posix)
# Success message
ifeq (posix, $(PLATFORM))
RES_STR := echo "\033[80G[\033[32mok\033[0m]"
else
RES_STR := echo -e "\033[80G[\033[32mok\033[0m]"
endif

# Create object files from C source files
$(BUILD_DIR)/%.o : %.c
	@$(PROGRESS) Compiling: $(notdir $<)
	$(NO_ECHO)$(CC) -c $(ALL_CFLAGS) $< -o $@ && $(RES_STR)

# Create object files from C++ source files
$(BUILD_DIR)/%.o : %.cpp
	@$(PROGRESS) Compiling: $(notdir $<)
	$(NO_ECHO)$(CC) -c $(ALL_CPPFLAGS) $< -o $@ && $(RES_STR)

# Create object files from assembly source files
define ASSEMBLING 
	$(NO_ECHO)$(CC) -c $(ALL_ASMFLAGS) $< -o $@ && $(RES_STR)
endef
$(BUILD_DIR)/%.o : %.s
	@$(PROGRESS) Assembling: $(notdir $<)
	$(ASSEMBLING)

$(BUILD_DIR)/%.o : %.S
	@$(PROGRESS) Assembling: $(notdir $<)
	$(ASSEMBLING)

ifeq (NRF52840, $(strip $(MCU)))
$(TARGET): $(addprefix $(OUTPUT_DIR)/$(TARGET), .elf .bin .hex .uf2)
else
$(TARGET): $(addprefix $(OUTPUT_DIR)/$(TARGET), .elf .bin .hex)
endif

ifneq ($(strip $(LIB_SRCS)),)
DEP_LIBS += $(addprefix $(OUTPUT_DIR)/$(TARGET), .a)
LIB_LIST = $(addprefix $(OUTPUT_DIR)/$(TARGET), .arlst)

$(TARGET): $(DEP_LIBS)

$(OUTPUT_DIR):
	+$(MK) $(OUTPUT_DIR)

# Create lib files
%.a: $(LIB_OBJS) | $(OUTPUT_DIR)
	$(info Archiving: $(notdir $@))
	$(file >$(LIB_LIST), $^)
	$(NO_ECHO)$(AR) $@ @$(LIB_LIST)

endif

# Create elf files 
%.elf: $(OBJS) $(DEP_LIBS)
	$(info Linking: $(notdir $@))
	$(NO_ECHO)$(CC) -o $@ $(ALL_LDFLAGS) $^ $(LIBS) $(DEP_LIBS)
	$(NO_ECHO)$(SIZE) $@

# Create binary .bin file from the .elf file
%.bin: %.elf
	$(info Creating: $(notdir $@))
	$(NO_ECHO)$(OBJCOPY) -O binary $< $@

# Create binary .hex file from the .elf file
%.hex: %.elf
	$(info Creating: $(notdir $@))
	$(NO_ECHO)$(OBJCOPY) -O ihex $< $@

# Create uf2 file from the .hex file
%.uf2: %.hex
	$(info Creating: $(notdir $@))
	$(NO_ECHO)$(UF2) $< -c -f 0xADA52840 -o $@

# Include the dependency files
-include $(OBJS:%.o=%.d)
