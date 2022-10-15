
SRCS += \
	$(VENDOR_DIR)/device_$(MCU_SERIES)/Source/Templates/gcc/startup_stm32l432xx.s \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash_ramfunc.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_dma_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pwr_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_adc.c \

APP_DEFS += \
	-DSTM32L432xx \
    -DCFG_TUSB_MCU=OPT_MCU_STM32L4 \

ifneq (yes, $(strip $(TINYUSB_USE_HAL)))
	SRCS += $(STM32SDK_DIR)/mcus/$(MCU_SERIES)/dcd_stm32_fsdev.c
endif

SDK_DEFS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

MCU_LD = STM32L432KBUx
