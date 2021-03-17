SRCS += 

APP_DEFS += \
	-D__SAMD21G18A__ \
    -DCFG_TUSB_MCU=OPT_MCU_ATSAMD21 \

SDK_DEFS += -mcpu=cortex-cortex-m0plus -mthumb -mlong-calls

MCU_LD = samd21g18a
