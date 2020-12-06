# Command alias
MK := mkdir -p
RM := rm -rf
CP := cp

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
AR      := $(GNU_INSTALL_ROOT)$(GNU_PREFIX)-ar -r
LD      := $(GNU_INSTALL_ROOT)$(GNU_PREFIX)-ld
NM      := $(GNU_INSTALL_ROOT)$(GNU_PREFIX)-nm
OBJDUMP := $(GNU_INSTALL_ROOT)$(GNU_PREFIX)-objdump
OBJCOPY := $(GNU_INSTALL_ROOT)$(GNU_PREFIX)-objcopy
SIZE    := $(GNU_INSTALL_ROOT)$(GNU_PREFIX)-size


BUILD_DIR := $(OUTPUT_DIR)/$(TARGET)

OBJS += $(patsubst %.s,$(BUILD_DIR)/%.o,$(patsubst %.S,$(BUILD_DIR)/%.o,$(filter %.s %.S,$(SRCS))))
OBJS += $(patsubst %.c,$(BUILD_DIR)/%.o,$(patsubst %.cpp,$(BUILD_DIR)/%.o,$(filter-out %.s %.S,$(SRCS))))

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

#$(info $(SRCS))
#$(info $(INCS))
#$(info $(LIBS))
#$(info $(OBJS))

ifeq (1,$(NO_DEBUG))
OPT = -Os -g3
else
OPT = -Og -gdwarf-2 -DDEBUG
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

# Success message
define RES_STR 
echo -ne "\033[60G[\033[32mok\033[0m]\n"
endef

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

$(TARGET): $(addprefix $(OUTPUT_DIR)/$(TARGET), .elf .bin .hex)

# Create elf files 
%.elf: $(OBJS)
	$(info Linking: $(notdir $@))
	$(NO_ECHO)$(CC) -o $@ $(ALL_LDFLAGS) $^ $(LIBS)
	$(NO_ECHO)$(SIZE) $@

# Create binary .bin file from the .elf file
%.bin: %.elf
	$(info Creating: $(notdir $@))
	$(NO_ECHO)$(OBJCOPY) -O binary $< $@

# Create binary .hex file from the .elf file
%.hex: %.elf
	$(info Creating: $(notdir $@))
	$(NO_ECHO)$(OBJCOPY) -O ihex $< $@

# Include the dependency files
-include $(OBJS:%.o=%.d)