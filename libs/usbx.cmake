
# core
set(USBX_CORE_DIR ${CMAKE_CURRENT_LIST_DIR}/rtos/usbx/common/core)

target_sources(${KEYBOARD}
    PRIVATE
	${USBX_CORE_DIR}/src/ux_device_stack_alternate_setting_get.c
	${USBX_CORE_DIR}/src/ux_device_stack_alternate_setting_set.c
	${USBX_CORE_DIR}/src/ux_device_stack_class_register.c
	${USBX_CORE_DIR}/src/ux_device_stack_class_unregister.c
	${USBX_CORE_DIR}/src/ux_device_stack_clear_feature.c
	${USBX_CORE_DIR}/src/ux_device_stack_configuration_get.c
	${USBX_CORE_DIR}/src/ux_device_stack_configuration_set.c
	${USBX_CORE_DIR}/src/ux_device_stack_control_request_process.c
	${USBX_CORE_DIR}/src/ux_device_stack_descriptor_send.c
	${USBX_CORE_DIR}/src/ux_device_stack_disconnect.c
	${USBX_CORE_DIR}/src/ux_device_stack_endpoint_stall.c
	${USBX_CORE_DIR}/src/ux_device_stack_get_status.c
	${USBX_CORE_DIR}/src/ux_device_stack_host_wakeup.c
	${USBX_CORE_DIR}/src/ux_device_stack_initialize.c
	${USBX_CORE_DIR}/src/ux_device_stack_interface_delete.c
	${USBX_CORE_DIR}/src/ux_device_stack_interface_get.c
	${USBX_CORE_DIR}/src/ux_device_stack_interface_set.c
	${USBX_CORE_DIR}/src/ux_device_stack_interface_start.c
	${USBX_CORE_DIR}/src/ux_device_stack_microsoft_extension_register.c
	${USBX_CORE_DIR}/src/ux_device_stack_set_feature.c
	${USBX_CORE_DIR}/src/ux_device_stack_transfer_abort.c
	${USBX_CORE_DIR}/src/ux_device_stack_transfer_all_request_abort.c
	${USBX_CORE_DIR}/src/ux_device_stack_transfer_request.c
	${USBX_CORE_DIR}/src/ux_device_stack_uninitialize.c
	${USBX_CORE_DIR}/src/ux_system_error_handler.c
	${USBX_CORE_DIR}/src/ux_system_initialize.c
	${USBX_CORE_DIR}/src/ux_system_uninitialize.c
	${USBX_CORE_DIR}/src/ux_trace_event_insert.c
	${USBX_CORE_DIR}/src/ux_trace_event_update.c
	${USBX_CORE_DIR}/src/ux_trace_object_register.c
	${USBX_CORE_DIR}/src/ux_trace_object_unregister.c
	${USBX_CORE_DIR}/src/ux_utility_debug_callback_register.c
	${USBX_CORE_DIR}/src/ux_utility_debug_log.c
	${USBX_CORE_DIR}/src/ux_utility_delay_ms.c
	${USBX_CORE_DIR}/src/ux_utility_descriptor_pack.c
	${USBX_CORE_DIR}/src/ux_utility_descriptor_parse.c
	${USBX_CORE_DIR}/src/ux_utility_error_callback_register.c
	${USBX_CORE_DIR}/src/ux_utility_event_flags_create.c
	${USBX_CORE_DIR}/src/ux_utility_event_flags_delete.c
	${USBX_CORE_DIR}/src/ux_utility_event_flags_get.c
	${USBX_CORE_DIR}/src/ux_utility_event_flags_set.c
	${USBX_CORE_DIR}/src/ux_utility_long_get.c
	${USBX_CORE_DIR}/src/ux_utility_long_get_big_endian.c
	${USBX_CORE_DIR}/src/ux_utility_long_put.c
	${USBX_CORE_DIR}/src/ux_utility_long_put_big_endian.c
	${USBX_CORE_DIR}/src/ux_utility_memory_allocate.c
	${USBX_CORE_DIR}/src/ux_utility_memory_allocate_add_safe.c
	${USBX_CORE_DIR}/src/ux_utility_memory_allocate_mulc_safe.c
	${USBX_CORE_DIR}/src/ux_utility_memory_allocate_mulv_safe.c
	${USBX_CORE_DIR}/src/ux_utility_memory_compare.c
	${USBX_CORE_DIR}/src/ux_utility_memory_copy.c
	${USBX_CORE_DIR}/src/ux_utility_memory_free.c
	${USBX_CORE_DIR}/src/ux_utility_memory_free_block_best_get.c
	${USBX_CORE_DIR}/src/ux_utility_memory_set.c
	${USBX_CORE_DIR}/src/ux_utility_mutex_create.c
	${USBX_CORE_DIR}/src/ux_utility_mutex_delete.c
	${USBX_CORE_DIR}/src/ux_utility_mutex_off.c
	${USBX_CORE_DIR}/src/ux_utility_mutex_on.c
	${USBX_CORE_DIR}/src/ux_utility_pci_class_scan.c
	${USBX_CORE_DIR}/src/ux_utility_pci_read.c
	${USBX_CORE_DIR}/src/ux_utility_pci_write.c
	${USBX_CORE_DIR}/src/ux_utility_physical_address.c
	${USBX_CORE_DIR}/src/ux_utility_semaphore_create.c
	${USBX_CORE_DIR}/src/ux_utility_semaphore_delete.c
	${USBX_CORE_DIR}/src/ux_utility_semaphore_get.c
	${USBX_CORE_DIR}/src/ux_utility_semaphore_put.c
	${USBX_CORE_DIR}/src/ux_utility_set_interrupt_handler.c
	${USBX_CORE_DIR}/src/ux_utility_short_get.c
	${USBX_CORE_DIR}/src/ux_utility_short_get_big_endian.c
	${USBX_CORE_DIR}/src/ux_utility_short_put.c
	${USBX_CORE_DIR}/src/ux_utility_short_put_big_endian.c
	${USBX_CORE_DIR}/src/ux_utility_string_length_check.c
	${USBX_CORE_DIR}/src/ux_utility_string_length_get.c
	${USBX_CORE_DIR}/src/ux_utility_string_to_unicode.c
	${USBX_CORE_DIR}/src/ux_utility_thread_create.c
	${USBX_CORE_DIR}/src/ux_utility_thread_delete.c
	${USBX_CORE_DIR}/src/ux_utility_thread_identify.c
	${USBX_CORE_DIR}/src/ux_utility_thread_relinquish.c
	${USBX_CORE_DIR}/src/ux_utility_thread_resume.c
	${USBX_CORE_DIR}/src/ux_utility_thread_schedule_other.c
	${USBX_CORE_DIR}/src/ux_utility_thread_sleep.c
	${USBX_CORE_DIR}/src/ux_utility_thread_suspend.c
	${USBX_CORE_DIR}/src/ux_utility_timer_create.c
	${USBX_CORE_DIR}/src/ux_utility_timer_delete.c
	${USBX_CORE_DIR}/src/ux_utility_unicode_to_string.c
	${USBX_CORE_DIR}/src/ux_utility_virtual_address.c
)

target_include_directories(${KEYBOARD}
    PUBLIC
    ${USBX_CORE_DIR}/inc
)

target_compile_definitions(${KEYBOARD} PUBLIC "UX_INCLUDE_USER_DEFINE_FILE")

# class 
set(USBX_CLASS_DIR ${CMAKE_CURRENT_LIST_DIR}/rtos/usbx/common/usbx_device_classes)

target_sources(${KEYBOARD}
    PRIVATE
	${USBX_CLASS_DIR}/src/ux_device_class_hid_activate.c
	${USBX_CLASS_DIR}/src/ux_device_class_hid_control_request.c
	${USBX_CLASS_DIR}/src/ux_device_class_hid_deactivate.c
	${USBX_CLASS_DIR}/src/ux_device_class_hid_descriptor_send.c
	${USBX_CLASS_DIR}/src/ux_device_class_hid_entry.c
	${USBX_CLASS_DIR}/src/ux_device_class_hid_event_get.c
	${USBX_CLASS_DIR}/src/ux_device_class_hid_event_set.c
	${USBX_CLASS_DIR}/src/ux_device_class_hid_initialize.c
	${USBX_CLASS_DIR}/src/ux_device_class_hid_interrupt_thread.c
	${USBX_CLASS_DIR}/src/ux_device_class_hid_report_get.c
	${USBX_CLASS_DIR}/src/ux_device_class_hid_report_set.c
	${USBX_CLASS_DIR}/src/ux_device_class_hid_uninitialize.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_activate.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_control_request.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_csw_send.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_deactivate.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_entry.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_format.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_get_configuration.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_get_performance.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_get_status_notification.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_initialize.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_inquiry.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_mode_select.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_mode_sense.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_prevent_allow_media_removal.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_read.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_read_capacity.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_read_disk_information.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_read_dvd_structure.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_read_format_capacity.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_read_toc.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_report_key.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_request_sense.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_start_stop.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_synchronize_cache.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_test_ready.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_thread.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_uninitialize.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_verify.c
	${USBX_CLASS_DIR}/src/ux_device_class_storage_write.c
)

target_include_directories(${KEYBOARD}
    PUBLIC
    ${USBX_CLASS_DIR}/inc
)

# dcd
set(USBX_DCD_DIR ${CMAKE_CURRENT_LIST_DIR}/rtos/usbx_stm32_device_controllers)
target_sources(${KEYBOARD}
    PRIVATE
	${USBX_DCD_DIR}/ux_dcd_stm32_callback.c
	${USBX_DCD_DIR}/ux_dcd_stm32_endpoint_create.c
	${USBX_DCD_DIR}/ux_dcd_stm32_endpoint_destroy.c
	${USBX_DCD_DIR}/ux_dcd_stm32_endpoint_reset.c
	${USBX_DCD_DIR}/ux_dcd_stm32_endpoint_stall.c
	${USBX_DCD_DIR}/ux_dcd_stm32_endpoint_status.c
	${USBX_DCD_DIR}/ux_dcd_stm32_frame_number_get.c
	${USBX_DCD_DIR}/ux_dcd_stm32_function.c
	${USBX_DCD_DIR}/ux_dcd_stm32_initialize_complete.c
	${USBX_DCD_DIR}/ux_dcd_stm32_initialize.c
	${USBX_DCD_DIR}/ux_dcd_stm32_interrupt_handler.c
	${USBX_DCD_DIR}/ux_dcd_stm32_transfer_request.c
	${USBX_DCD_DIR}/ux_dcd_stm32_uninitialize.c
)

target_include_directories(${KEYBOARD}
    PUBLIC
    ${USBX_DCD_DIR}
)

# ports
target_include_directories(${KEYBOARD}
	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/rtos/usbx/ports/cortex_${MCU_ARCH}/gnu/inc
)
