
SRCS += \
	$(VENDOR_DIR)/device_$(MCU_SERIES)/Source/Templates/gcc/startup_stm32f412rx.s \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_flash_ramfunc.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_dma_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_pwr_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_i2c.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_i2c_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_spi.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_adc.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_adc_ex.c \
	$(VENDOR_DIR)/driver_$(MCU_SERIES)/Src/$(MCU_FAMILY)_hal_qspi.c \

APP_DEFS += \
	-DSTM32F412Rx \
    -DCFG_TUSB_MCU=OPT_MCU_STM32F4 \

SDK_DEFS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

MCU_LD = STM32F412RETx

ifneq (yes, $(strip $(TINYUSB_USE_HAL)))
	SRCS += $(TINYUSB_DIR)/portable/synopsys/dwc2/dcd_dwc2.c
	INCS += $(TINYUSB_DIR)/portable/synopsys/dwc2
endif
