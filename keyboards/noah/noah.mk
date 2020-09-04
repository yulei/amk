
NOAH_DIR := $(TOP_DIR)/keyboards/noah

SRC_FILES += \
	$(NOAH_DIR)/noah.c \
	$(NOAH_DIR)/noah_keymap.c \

INC_FOLDERS += \
	$(NOAH_DIR) \

MCU = STM32F411 
BOOTMAGIC_ENABLE = yes
MOUSEKEY_ENABLE = yes
EXTRAKEY_ENABLE = yes
RGB_EFFECTS_ENABLE = yes