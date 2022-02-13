USBX_HCD_DIR := $(LIB_DIR)/rtos/usbx_stm32_host_controllers

INCS += $(USBX_HCD_DIR)
SRCS += \
	$(USBX_HCD_DIR)/ux_hcd_stm32_callback.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_controller_disable.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_ed_obtain.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_endpoint_create.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_endpoint_destroy.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_endpoint_reset.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_entry.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_frame_number_get.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_initialize.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_interrupt_handler.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_least_traffic_list_get.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_periodic_schedule.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_port_disable.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_port_enable.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_port_reset.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_port_resume.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_port_status_get.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_port_suspend.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_power_down_port.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_power_on_port.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_request_bulk_transfer.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_request_control_transfer.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_request_transfer.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_transfer_abort.c \
	$(USBX_HCD_DIR)/ux_hcd_stm32_request_periodic_transfer.c \
