
NUVOTONSDK_DIR := $(VENDOR_DIR)
MCU_FAMILY := nuc126

SRCS += \
	$(NUVOTONSDK_DIR)/$(MCU_FAMILY)/Library/StdDriver/src/clk.c \
	$(NUVOTONSDK_DIR)/$(MCU_FAMILY)/Library/StdDriver/src/fmc.c \
	$(NUVOTONSDK_DIR)/$(MCU_FAMILY)/Library/StdDriver/src/gpio.c \
	$(NUVOTONSDK_DIR)/$(MCU_FAMILY)/Library/StdDriver/src/sys.c \
	$(NUVOTONSDK_DIR)/$(MCU_FAMILY)/Library/StdDriver/src/uart.c \
	$(NUVOTONSDK_DIR)/$(MCU_FAMILY)/Library/Device/Nuvoton/NUC126/Source/system_NUC126.c \
	$(NUVOTONSDK_DIR)/$(MCU_FAMILY)/Library/Device/Nuvoton/NUC126/Source/gcc/startup_NUC126.S \

INCS += \
	$(NUVOTONSDK_DIR) \
	$(NUVOTONSDK_DIR)/$(MCU_FAMILY)/Library/StdDriver/inc \
	$(NUVOTONSDK_DIR)/$(MCU_FAMILY)/Library/Device/Nuvoton/NUC126/Include \
	$(NUVOTONSDK_DIR)/$(MCU_FAMILY)/Library/CMSIS/Include \

ifeq (yes, $(strip $(TINYUSB_ENABLE)))
	include $(LIB_DIR)/tinyusb.mk
	APP_DEFS += -DTINYUSB_ENABLE
endif

APP_DEFS += -DNUC126 -DCFG_TUSB_MCU=OPT_MCU_NUC126

LINKER_PATH	?= $(NUVOTONSDK_DIR)/$(MCU_FAMILY)/Library/Device/Nuvoton/NUC126/Source/gcc
LINKER_SCRIPT := $(LINKER_PATH)/gcc_arm.ld

SDK_DEFS += -mcpu=cortex-m0 -mthumb