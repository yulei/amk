PLATFORM_PORTING_DIR := $(MAIN_DIR)/porting/wired/hc32

SRCS += \
	$(PLATFORM_PORTING_DIR)/gpio_pin.c \
	$(PLATFORM_PORTING_DIR)/hc32.c \
	$(PLATFORM_PORTING_DIR)/hc32f460.c \
	$(MAIN_DIR)/porting/wired/common/cm_misc.c \

INCS += \
	$(PLATFORM_PORTING_DIR) \
