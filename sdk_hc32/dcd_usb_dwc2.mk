
SRCS += \
	$(HC32SDK_DIR)/dcd_dwc2.c \

INCS += \
	$(HC32SDK_DIR) \

APP_DEFS += \
	-DTUP_DCD_ENDPOINT_MAX=6 \
