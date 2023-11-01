PLATFORM_PORTING_DIR := $(MAIN_DIR)/porting/wired/m480

SRCS += \
	$(PLATFORM_PORTING_DIR)/gpio_pin.c \
	$(PLATFORM_PORTING_DIR)/m480.c \
	$(PLATFORM_PORTING_DIR)/m484.c \
	$(MAIN_DIR)/porting/wired/common/cm_misc.c \

INCS += \
	$(PLATFORM_PORTING_DIR) \
