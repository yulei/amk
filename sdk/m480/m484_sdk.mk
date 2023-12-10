
SRCS += \

APP_DEFS += \
	-DM484SIDEA \
	-DM484 \
    -DCFG_TUSB_MCU=OPT_MCU_M484 \

SDK_DEFS += -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16

MCU_LD := m484sidea
