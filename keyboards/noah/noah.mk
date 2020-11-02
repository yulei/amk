
NOAH_DIR := $(TOP_DIR)/keyboards/noah

SRCS += \
	$(NOAH_DIR)/noah.c \
	$(NOAH_DIR)/noah_keymap.c \

INCS += \
	$(NOAH_DIR) \

MCU = STM32F411 
BOOTMAGIC_ENABLE = yes
#RGB_EFFECTS_ENABLE = yes