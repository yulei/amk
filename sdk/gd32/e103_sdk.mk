
SRCS += \
	$(GD32SDK_DIR)/startup_gd32e103.s \
	$(GD32SDK_DIR)/system_gd32e10x.c \
	$(GD32SDK_DIR)/gd32e10x_it.c \
	$(GD32SDK_DIR)/gd32_util.c \

APP_DEFS += \
	-DUSE_STDPERIPH_DRIVER \
    -DGD32E10X \
	-DGD32E103 \
	-DHXTAL_VALUE=16000000 \
	-DHXTAL_VALUE_16M=16000000 \
	-DTUP_DCD_ENDPOINT_MAX=4 \
    -DCFG_TUSB_MCU=OPT_MCU_GD32E103 \

SDK_DEFS += -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16

MCU_LD := gd32e103xb
