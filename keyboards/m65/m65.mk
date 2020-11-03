
CUR_KBD := m65

SRCS += \
	keyboards/$(CUR_KBD)/$(CUR_KBD).c \
	keyboards/$(CUR_KBD)/$(CUR_KBD)_keymap.c \

INCS += \
	keyboards/$(CUR_KBD) \

MCU = STM32F411 
BOOTMAGIC_ENABLE = yes
SCREEN_ENABLE = yes