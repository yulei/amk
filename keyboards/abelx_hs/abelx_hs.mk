
SRCS += \
	$(KEYBOARD_DIR)/abelx_hs.c \
	$(KEYBOARD_DIR)/abelx_hs_keymap.c

MCU = NRF52832
#RGB_EFFECTS_ENABLE = yes 
EECONFIG_FRAM = yes
#EECONFIG_FDS = yes
MATRIX_USE_PCA9535 = yes
RGB_INDICATOR_ENABLE = yes
RGB_LINEAR_ENABLE = yes