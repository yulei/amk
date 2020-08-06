TINYUSB_DIR := $(TOP_DIR)/tinyusb

SRC_FILES += \

INC_FOLDERS += \
	$(TINYUSB_DIR) \
	$(TINYUSB_DIR)/tinyusb/src \

ifeq (NRF52832, $(strip $(MCU)))
	APP_DEFS += -DCFG_TUSB_MCU=OPT_MCU_NRF5X
endif

ifeq (STM32F411, $(strip $(MCU)))
	SRC_FILES += $(TINYUSB_DIR)/tinyusb/src/tusb.c 
	SRC_FILES += $(TINYUSB_DIR)/tinyusb/src/common/tusb_fifo.c
	SRC_FILES += $(TINYUSB_DIR)/tinyusb/src/device/usbd.c
	SRC_FILES += $(TINYUSB_DIR)/tinyusb/src/device/usbd_control.c
	SRC_FILES += $(TINYUSB_DIR)/tinyusb/src/class/cdc/cdc_device.c
	SRC_FILES += $(TINYUSB_DIR)/tinyusb/src/class/dfu/dfu_rt_device.c
	SRC_FILES += $(TINYUSB_DIR)/tinyusb/src/class/hid/hid_device.c
	SRC_FILES += $(TINYUSB_DIR)/tinyusb/src/class/midi/midi_device.c
	SRC_FILES += $(TINYUSB_DIR)/tinyusb/src/class/msc/msc_device.c
	SRC_FILES += $(TINYUSB_DIR)/tinyusb/src/class/net/net_device.c
	SRC_FILES += $(TINYUSB_DIR)/tinyusb/src/class/usbtmc/usbtmc_device.c
	SRC_FILES += $(TINYUSB_DIR)/tinyusb/src/class/vendor/vendor_device.c
	SRC_FILES += $(TINYUSB_DIR)/tinyusb/src/portable/st/synopsys/dcd_synopsys.c
	APP_DEFS += -DCFG_TUSB_MCU=OPT_MCU_STM32F4
endif
