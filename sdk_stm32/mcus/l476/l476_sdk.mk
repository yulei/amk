
SRCS += \
	$(VENDOR_DIR)/device_$(MCU_SERIES)/Source/Templates/gcc/startup_stm32l476xx.s \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash_ramfunc.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_dma_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pwr_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_adc.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_adc_ex.c \

APP_DEFS += \
	-DSTM32L476xx \
    -DCFG_TUSB_MCU=OPT_MCU_STM32L4 \
	-DSYSTEM_CLOCK=80000000


SDK_DEFS += -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16

MCU_LD = STM32L476RETx
