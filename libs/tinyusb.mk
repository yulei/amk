TINYUSB_DIR := $(LIB_DIR)/tinyusb/src

SRCS += \
	$(TINYUSB_DIR)/tusb.c \
	$(TINYUSB_DIR)/class/hid/hid_device.c \
	$(TINYUSB_DIR)/class/audio/audio_device.c \
	$(TINYUSB_DIR)/class/msc/msc_device.c \
	$(TINYUSB_DIR)/class/cdc/cdc_device.c \
	$(TINYUSB_DIR)/class/vendor/vendor_device.c \
	$(TINYUSB_DIR)/common/tusb_fifo.c \
	$(TINYUSB_DIR)/device/usbd.c \
	$(TINYUSB_DIR)/device/usbd_control.c \

INCS += \
	$(TINYUSB_DIR) \

ifeq (yes, $(strip $(TINYUSB_USE_HAL)))
	SRCS += $(STM32SDK_DIR)/hal_usb/dcd_hal_stm32.c
else
	ifeq (STM32F405, $(strip $(MCU)))
		ifeq (yes, $(strip $(USE_F405_APM)))
			#ifeq (yes, $(strip $(RTOS_ENABLE)))
				SRCS += $(STM32SDK_DIR)/mcus/f405_apm/dcd_dwc2_apm32.c
			#else
			#	SRCS += $(STM32SDK_DIR)/mcus/f405_apm/dcd_hal_apm32.c
			#endif
		endif
	endif	
	ifeq (STM32F103, $(strip $(MCU)))
	SRCS += $(TINYUSB_DIR)/portable/st/stm32_fsdev/dcd_stm32_fsdev.c
	endif

	ifeq (STM32L432, $(strip $(MCU)))
	SRCS += $(TINYUSB_DIR)/portable/st/stm32_fsdev/dcd_stm32_fsdev.c
	endif

	ifeq (STM32G431, $(strip $(MCU)))
	SRCS += $(TINYUSB_DIR)/portable/st/stm32_fsdev/dcd_stm32_fsdev.c
	endif

	ifeq (STM32L476, $(strip $(MCU)))
	INCS +=$(TINYUSB_DIR)/portable/synopsys/dwc2
	SRCS += $(TINYUSB_DIR)/portable/synopsys/dwc2/dcd_dwc2.c
	endif

	ifeq (STM32U575, $(strip $(MCU)))
	INCS +=$(TINYUSB_DIR)/portable/synopsys/dwc2
	SRCS += $(TINYUSB_DIR)/portable/synopsys/dwc2/dcd_dwc2.c
	endif

	ifeq (STM32F411, $(strip $(MCU)))
	SRCS += $(TINYUSB_DIR)/portable/st/synopsys/dcd_synopsys.c
	endif

	ifeq (STM32F722, $(strip $(MCU)))
	SRCS += $(TINYUSB_DIR)/portable/st/synopsys/dcd_synopsys.c
	endif

	ifeq (ATSAMD21, $(strip $(MCU)))
	SRCS += $(TINYUSB_DIR)/portable/microchip/samd/dcd_samd.c
	endif

	ifeq (NUC126, $(strip $(MCU)))
	SRCS += $(TINYUSB_DIR)/portable/nuvoton/nuc121/dcd_nuc121.c
	endif
endif #TINYUSB_USE_HAL

ifeq (NRF52840, $(strip $(MCU)))
SRCS += $(SDK_NRF5)/dcd_nrf5x.c
endif

APP_DEFS += -DTINYUSB_ENABLE