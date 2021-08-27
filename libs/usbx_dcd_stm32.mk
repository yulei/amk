USBX_DCD_DIR := $(LIB_DIR)/rtos/usbx_stm32_device_controllers

INCS += $(USBX_DCD_DIR)
SRCS += \
	$(USBX_DCD_DIR)/ux_dcd_stm32_callback.c \
	$(USBX_DCD_DIR)/ux_dcd_stm32_endpoint_create.c \
	$(USBX_DCD_DIR)/ux_dcd_stm32_endpoint_destroy.c \
	$(USBX_DCD_DIR)/ux_dcd_stm32_endpoint_reset.c \
	$(USBX_DCD_DIR)/ux_dcd_stm32_endpoint_stall.c \
	$(USBX_DCD_DIR)/ux_dcd_stm32_endpoint_status.c \
	$(USBX_DCD_DIR)/ux_dcd_stm32_frame_number_get.c \
	$(USBX_DCD_DIR)/ux_dcd_stm32_function.c \
	$(USBX_DCD_DIR)/ux_dcd_stm32_initialize_complete.c \
	$(USBX_DCD_DIR)/ux_dcd_stm32_initialize.c \
	$(USBX_DCD_DIR)/ux_dcd_stm32_interrupt_handler.c \
	$(USBX_DCD_DIR)/ux_dcd_stm32_transfer_request.c \
	$(USBX_DCD_DIR)/ux_dcd_stm32_uninitialize.c \
