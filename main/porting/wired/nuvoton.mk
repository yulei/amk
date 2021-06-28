
NUVOTON_PORTING_DIR := $(MAIN_DIR)/porting/wired/nuvoton

SRCS += \
	$(NUVOTON_PORTING_DIR)/gpio_pin.c \
	$(NUVOTON_PORTING_DIR)/nuvoton.c \
	$(NUVOTON_PORTING_DIR)/wait.c \

INCS += \
	$(NUVOTON_PORTING_DIR) \
