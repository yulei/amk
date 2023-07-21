
SRCS += \
	$(APM32SDK_DIR)/$(MCU_TYPE)/startup_apm32f40x.S \
	$(APM32SDK_DIR)/$(MCU_TYPE)/system_$(MCU_SERIES).c \

APP_DEFS += \
	-DCFG_TUSB_MCU=OPT_MCU_APM32F40X \
    -DAPM32F407 \
    -DAPM32F40X \
	-DBOARD_TUD_MAX_SPEED=OPT_MODE_HIGH_SPEED

SDK_DEFS += -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16

MCU_LD := APM32F4xxxG
