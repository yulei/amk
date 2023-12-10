
SRCS += \
	$(AT32SDK_DIR)/$(MCU_TYPE)/startup_$(MCU_SERIES).s \
	$(AT32SDK_DIR)/$(MCU_TYPE)/system_$(MCU_SERIES).c \

APP_DEFS += \
	-DCFG_TUSB_MCU=OPT_MCU_AT32F405 \
    -DAT32F405RCT7 \
    -DAT32F405 \
	-DBOARD_TUD_MAX_SPEED=OPT_MODE_HIGH_SPEED

SDK_DEFS += -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16

MCU_LD := AT32F405xC
