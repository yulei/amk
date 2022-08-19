
SRCS += \
	$(GD32SDK_DIR)/$(MCU_TYPE)/dcd_dwc2.c \

INCS += \
	$(GD32SDK_DIR)/$(MCU_TYPE) \

APP_DEFS += \
	-DTUP_DCD_ENDPOINT_MAX=6 \
