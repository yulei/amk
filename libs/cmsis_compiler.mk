CMSIS_COMPILER_DIR := $(LIB_DIR)/cmsis_compiler

INCS += \
	$(CMSIS_COMPILER_DIR)/include \

SRCS += \
	$(CMSIS_COMPILER_DIR)/source/gcc/retarget_syscalls.c \

