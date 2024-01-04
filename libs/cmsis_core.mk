CMSIS_CORE_DIR := $(LIB_DIR)/cmsis_6/CMSIS/Core

INCS += \
	$(CMSIS_CORE_DIR)/Include \

include $(LIB_DIR)/cmsis_compiler.mk
