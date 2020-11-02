
MK := mkdir -p
RM := rm -rf
CP := cp

# echo suspend
ifeq ($(VERBOSE),1)
  NO_ECHO :=
else
  NO_ECHO := @
endif

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

.PHONY: all clean

all: default

clean:
	$(RM) $(OUTPUT_DIRECTORY)

BUILD_DIR := $(OUTPUT_DIRECTORY)/$(TARGET)

OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(patsubst %.cpp,$(BUILD_DIR)/%.o,$(patsubst %.s,$(BUILD_DIR)/%.o,$(patsubst %.S,$(BUILD_DIR)/%.o,$(SRCS)))))

# Create build directories
OBJ_DIRS = $(sort $(dir $(OBJ)))
$(OBJ): | $(OBJ_DIRS)
$(OBJ_DIRS):
	$(NO_ECHO)$(MK) $@

#$(info $(SRC_FILES))
#$(info $(OBJ))

# Compiler flags to generate dependency files.
GENDEPFLAGS = -MD 

INC_FLAGS = $(addprefix -I,$(INCS))

ALL_CFLAGS = -std=gnu99 $(CFLAGS) $(GENDEPFLAGS) $(INC_FLAGS)
ALL_CPPFLAGS = -x c++ $(CPPFLAGS) $(GENDEPFLAGS) $(INC_FLAGS)
ALL_ASFLAGS = -x assembler-with-cpp $(ASMFLAGS) $(INC_FLAGS)
ALL_LDFLAGS = $(LDFLAGS) -Xlinker -Map=$(OUTPUT_DIRECTORY)/$(TARGET).map
# Create object files from C source files

$(BUILD_DIR)/%.o : %.c
	@echo Compiling: $<
	$(NO_ECHO)$(CC) -c $(ALL_CFLAGS) $< -o $@ 

# Create object files from C++ source files
$(BUILD_DIR)/%.o : %.cpp
	@echo Compiling: $<
	$(NO_ECHO)$(CC) -c $(ALL_CPPFLAGS) $< -o $@ 

# Create object files from assembly source files
define assembling
	$(NO_ECHO)$(CC) -c $(ALL_ASFLAGS) $< -o $@
endef
$(BUILD_DIR)/%.o : %.s
	@echo Assembling: $<
	$(assembling)

$(BUILD_DIR)/%.o : %.S
	@echo Assembling: $<
	$(assembling)

# Link object files
$(TARGET): $(addprefix $(OUTPUT_DIRECTORY)/$(TARGET), .elf .bin .hex)

%.elf: $(OBJ)
	$(info Preparing: $(notdir $@))
	$(NO_ECHO)$(CC) -o $@ $(ALL_LDFLAGS) $^ $(LIBS)

# Create binary .bin file from the .out file
%.bin: %.elf
	$(info Preparing: $(notdir $@))
	$(NO_ECHO)$(OBJCOPY) -O binary $< $@

# Create binary .hex file from the .out file
%.hex: %.elf
	$(info Preparing: $(notdir $@))
	$(NO_ECHO)$(OBJCOPY) -O ihex $< $@
