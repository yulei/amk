
SRCS += \
	$(VENDOR_DIR)/device_$(MCU_SERIES)/Source/Templates/gcc/startup_stm32f401xc.s \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash_ramfunc.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_dma_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pwr_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_i2c.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_i2c_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_spi.c \

APP_DEFS += \
	-DSTM32F401xC \
    -DCFG_TUSB_MCU=OPT_MCU_STM32F4 \
	-DSYSTEM_CLOCK=84000000


SDK_DEFS += -mcpu=cortex-m4 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv4-sp-d16

MCU_LD = STM32F401RCTx