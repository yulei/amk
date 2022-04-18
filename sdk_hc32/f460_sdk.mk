
SRCS += \

APP_DEFS += \
	-DHC32F460xE \
	-DHC32F460 \
    -DCFG_TUSB_MCU=OPT_MCU_HC32F4 \

SDK_DEFS += -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16

MCU_LD := hc32f460xe