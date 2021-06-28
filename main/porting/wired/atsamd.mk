

ATSAMD_PORTING_DIR := $(MAIN_DIR)/porting/wired/atsamd

SRCS += \
	$(ATSAMD_PORTING_DIR)/atsamd.c \
	$(ATSAMD_PORTING_DIR)/gpio_pin.c \
	$(ATSAMD_PORTING_DIR)/wait.c \

INCS += \
	$(ATSAMD_PORTING_DIR) \
