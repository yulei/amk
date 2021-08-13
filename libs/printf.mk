PRINTF_DIR := $(LIB_DIR)/printf

SRCS += \
	$(PRINTF_DIR)/printf.c \

INCS += \
	$(PRINTF_DIR) \

APP_DEFS += -DPRINTF_INCLUDE_CONFIG_H