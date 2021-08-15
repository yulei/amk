THREADX_DIR := $(LIB_DIR)/rtos/threadx

INCS += $(THREADX_DIR)/common/inc

LIB_SRCS += \
	${THREADX_DIR}/common/src/tx_thread_delete.c \
	${THREADX_DIR}/common/src/tx_thread_reset.c

LIB_SRCS += \
	${THREADX_DIR}/common/src/tx_block_allocate.c \
	${THREADX_DIR}/common/src/tx_block_pool_cleanup.c \
	${THREADX_DIR}/common/src/tx_block_pool_create.c \
	${THREADX_DIR}/common/src/tx_block_pool_delete.c \
	${THREADX_DIR}/common/src/tx_block_pool_info_get.c \
	${THREADX_DIR}/common/src/tx_block_pool_initialize.c \
	${THREADX_DIR}/common/src/tx_block_pool_performance_info_get.c \
	${THREADX_DIR}/common/src/tx_block_pool_performance_system_info_get.c \
	${THREADX_DIR}/common/src/tx_block_pool_prioritize.c \
	${THREADX_DIR}/common/src/tx_block_release.c \
	${THREADX_DIR}/common/src/tx_byte_allocate.c \
	${THREADX_DIR}/common/src/tx_byte_pool_cleanup.c \
	${THREADX_DIR}/common/src/tx_byte_pool_create.c \
	${THREADX_DIR}/common/src/tx_byte_pool_delete.c \
	${THREADX_DIR}/common/src/tx_byte_pool_info_get.c \
	${THREADX_DIR}/common/src/tx_byte_pool_initialize.c \
	${THREADX_DIR}/common/src/tx_byte_pool_performance_info_get.c \
	${THREADX_DIR}/common/src/tx_byte_pool_performance_system_info_get.c \
	${THREADX_DIR}/common/src/tx_byte_pool_prioritize.c \
	${THREADX_DIR}/common/src/tx_byte_pool_search.c \
	${THREADX_DIR}/common/src/tx_byte_release.c \
	${THREADX_DIR}/common/src/tx_event_flags_cleanup.c \
	${THREADX_DIR}/common/src/tx_event_flags_create.c \
	${THREADX_DIR}/common/src/tx_event_flags_delete.c \
	${THREADX_DIR}/common/src/tx_event_flags_get.c \
	${THREADX_DIR}/common/src/tx_event_flags_info_get.c \
	${THREADX_DIR}/common/src/tx_event_flags_initialize.c \
	${THREADX_DIR}/common/src/tx_event_flags_performance_info_get.c \
	${THREADX_DIR}/common/src/tx_event_flags_performance_system_info_get.c \
	${THREADX_DIR}/common/src/tx_event_flags_set.c \
	${THREADX_DIR}/common/src/tx_event_flags_set_notify.c \
	${THREADX_DIR}/common/src/tx_initialize_high_level.c \
	${THREADX_DIR}/common/src/tx_initialize_kernel_enter.c \
	${THREADX_DIR}/common/src/tx_initialize_kernel_setup.c \
	${THREADX_DIR}/common/src/tx_misra.c \
	${THREADX_DIR}/common/src/tx_mutex_cleanup.c \
	${THREADX_DIR}/common/src/tx_mutex_create.c \
	${THREADX_DIR}/common/src/tx_mutex_delete.c \
	${THREADX_DIR}/common/src/tx_mutex_get.c \
	${THREADX_DIR}/common/src/tx_mutex_info_get.c \
	${THREADX_DIR}/common/src/tx_mutex_initialize.c \
	${THREADX_DIR}/common/src/tx_mutex_performance_info_get.c \
	${THREADX_DIR}/common/src/tx_mutex_performance_system_info_get.c \
	${THREADX_DIR}/common/src/tx_mutex_prioritize.c \
	${THREADX_DIR}/common/src/tx_mutex_priority_change.c \
	${THREADX_DIR}/common/src/tx_mutex_put.c \
	${THREADX_DIR}/common/src/tx_queue_cleanup.c \
	${THREADX_DIR}/common/src/tx_queue_create.c \
	${THREADX_DIR}/common/src/tx_queue_delete.c \
	${THREADX_DIR}/common/src/tx_queue_flush.c \
	${THREADX_DIR}/common/src/tx_queue_front_send.c \
	${THREADX_DIR}/common/src/tx_queue_info_get.c \
	${THREADX_DIR}/common/src/tx_queue_initialize.c \
	${THREADX_DIR}/common/src/tx_queue_performance_info_get.c \
	${THREADX_DIR}/common/src/tx_queue_performance_system_info_get.c \
	${THREADX_DIR}/common/src/tx_queue_prioritize.c \
	${THREADX_DIR}/common/src/tx_queue_receive.c \
	${THREADX_DIR}/common/src/tx_queue_send.c \
	${THREADX_DIR}/common/src/tx_queue_send_notify.c \
	${THREADX_DIR}/common/src/tx_semaphore_ceiling_put.c \
	${THREADX_DIR}/common/src/tx_semaphore_cleanup.c \
	${THREADX_DIR}/common/src/tx_semaphore_create.c \
	${THREADX_DIR}/common/src/tx_semaphore_delete.c \
	${THREADX_DIR}/common/src/tx_semaphore_get.c \
	${THREADX_DIR}/common/src/tx_semaphore_info_get.c \
	${THREADX_DIR}/common/src/tx_semaphore_initialize.c \
	${THREADX_DIR}/common/src/tx_semaphore_performance_info_get.c \
	${THREADX_DIR}/common/src/tx_semaphore_performance_system_info_get.c \
	${THREADX_DIR}/common/src/tx_semaphore_prioritize.c \
	${THREADX_DIR}/common/src/tx_semaphore_put.c \
	${THREADX_DIR}/common/src/tx_semaphore_put_notify.c \
	${THREADX_DIR}/common/src/tx_thread_create.c \
	${THREADX_DIR}/common/src/tx_thread_entry_exit_notify.c \
	${THREADX_DIR}/common/src/tx_thread_identify.c \
	${THREADX_DIR}/common/src/tx_thread_info_get.c \
	${THREADX_DIR}/common/src/tx_thread_initialize.c \
	${THREADX_DIR}/common/src/tx_thread_performance_info_get.c \
	${THREADX_DIR}/common/src/tx_thread_performance_system_info_get.c \
	${THREADX_DIR}/common/src/tx_thread_preemption_change.c \
	${THREADX_DIR}/common/src/tx_thread_priority_change.c \
	${THREADX_DIR}/common/src/tx_thread_relinquish.c \
	${THREADX_DIR}/common/src/tx_thread_resume.c \
	${THREADX_DIR}/common/src/tx_thread_shell_entry.c \
	${THREADX_DIR}/common/src/tx_thread_sleep.c \
	${THREADX_DIR}/common/src/tx_thread_stack_analyze.c \
	${THREADX_DIR}/common/src/tx_thread_stack_error_handler.c \
	${THREADX_DIR}/common/src/tx_thread_stack_error_notify.c \
	${THREADX_DIR}/common/src/tx_thread_suspend.c \
	${THREADX_DIR}/common/src/tx_thread_system_preempt_check.c \
	${THREADX_DIR}/common/src/tx_thread_system_resume.c \
	${THREADX_DIR}/common/src/tx_thread_system_suspend.c \
	${THREADX_DIR}/common/src/tx_thread_terminate.c \
	${THREADX_DIR}/common/src/tx_thread_time_slice.c \
	${THREADX_DIR}/common/src/tx_thread_time_slice_change.c \
	${THREADX_DIR}/common/src/tx_thread_timeout.c \
	${THREADX_DIR}/common/src/tx_thread_wait_abort.c \
	${THREADX_DIR}/common/src/tx_time_get.c \
	${THREADX_DIR}/common/src/tx_time_set.c \
	${THREADX_DIR}/common/src/tx_timer_activate.c \
	${THREADX_DIR}/common/src/tx_timer_change.c \
	${THREADX_DIR}/common/src/tx_timer_create.c \
	${THREADX_DIR}/common/src/tx_timer_deactivate.c \
	${THREADX_DIR}/common/src/tx_timer_delete.c \
	${THREADX_DIR}/common/src/tx_timer_expiration_process.c \
	${THREADX_DIR}/common/src/tx_timer_info_get.c \
	${THREADX_DIR}/common/src/tx_timer_initialize.c \
	${THREADX_DIR}/common/src/tx_timer_performance_info_get.c \
	${THREADX_DIR}/common/src/tx_timer_performance_system_info_get.c \
	${THREADX_DIR}/common/src/tx_timer_system_activate.c \
	${THREADX_DIR}/common/src/tx_timer_system_deactivate.c \
	${THREADX_DIR}/common/src/tx_timer_thread_entry.c \
	${THREADX_DIR}/common/src/tx_trace_buffer_full_notify.c \
	${THREADX_DIR}/common/src/tx_trace_disable.c \
	${THREADX_DIR}/common/src/tx_trace_enable.c \
	${THREADX_DIR}/common/src/tx_trace_event_filter.c \
	${THREADX_DIR}/common/src/tx_trace_event_unfilter.c \
	${THREADX_DIR}/common/src/tx_trace_initialize.c \
	${THREADX_DIR}/common/src/tx_trace_interrupt_control.c \
	${THREADX_DIR}/common/src/tx_trace_isr_enter_insert.c \
	${THREADX_DIR}/common/src/tx_trace_isr_exit_insert.c \
	${THREADX_DIR}/common/src/tx_trace_object_register.c \
	${THREADX_DIR}/common/src/tx_trace_object_unregister.c \
	${THREADX_DIR}/common/src/tx_trace_user_event_insert.c \
	${THREADX_DIR}/common/src/txe_block_allocate.c \
	${THREADX_DIR}/common/src/txe_block_pool_create.c \
	${THREADX_DIR}/common/src/txe_block_pool_delete.c \
	${THREADX_DIR}/common/src/txe_block_pool_info_get.c \
	${THREADX_DIR}/common/src/txe_block_pool_prioritize.c \
	${THREADX_DIR}/common/src/txe_block_release.c \
	${THREADX_DIR}/common/src/txe_byte_allocate.c \
	${THREADX_DIR}/common/src/txe_byte_pool_create.c \
	${THREADX_DIR}/common/src/txe_byte_pool_delete.c \
	${THREADX_DIR}/common/src/txe_byte_pool_info_get.c \
	${THREADX_DIR}/common/src/txe_byte_pool_prioritize.c \
	${THREADX_DIR}/common/src/txe_byte_release.c \
	${THREADX_DIR}/common/src/txe_event_flags_create.c \
	${THREADX_DIR}/common/src/txe_event_flags_delete.c \
	${THREADX_DIR}/common/src/txe_event_flags_get.c \
	${THREADX_DIR}/common/src/txe_event_flags_info_get.c \
	${THREADX_DIR}/common/src/txe_event_flags_set.c \
	${THREADX_DIR}/common/src/txe_event_flags_set_notify.c \
	${THREADX_DIR}/common/src/txe_mutex_create.c \
	${THREADX_DIR}/common/src/txe_mutex_delete.c \
	${THREADX_DIR}/common/src/txe_mutex_get.c \
	${THREADX_DIR}/common/src/txe_mutex_info_get.c \
	${THREADX_DIR}/common/src/txe_mutex_prioritize.c \
	${THREADX_DIR}/common/src/txe_mutex_put.c \
	${THREADX_DIR}/common/src/txe_queue_create.c \
	${THREADX_DIR}/common/src/txe_queue_delete.c \
	${THREADX_DIR}/common/src/txe_queue_flush.c \
	${THREADX_DIR}/common/src/txe_queue_front_send.c \
	${THREADX_DIR}/common/src/txe_queue_info_get.c \
	${THREADX_DIR}/common/src/txe_queue_prioritize.c \
	${THREADX_DIR}/common/src/txe_queue_receive.c \
	${THREADX_DIR}/common/src/txe_queue_send.c \
	${THREADX_DIR}/common/src/txe_queue_send_notify.c \
	${THREADX_DIR}/common/src/txe_semaphore_ceiling_put.c \
	${THREADX_DIR}/common/src/txe_semaphore_create.c \
	${THREADX_DIR}/common/src/txe_semaphore_delete.c \
	${THREADX_DIR}/common/src/txe_semaphore_get.c \
	${THREADX_DIR}/common/src/txe_semaphore_info_get.c \
	${THREADX_DIR}/common/src/txe_semaphore_prioritize.c \
	${THREADX_DIR}/common/src/txe_semaphore_put.c \
	${THREADX_DIR}/common/src/txe_semaphore_put_notify.c \
	${THREADX_DIR}/common/src/txe_thread_create.c \
	${THREADX_DIR}/common/src/txe_thread_delete.c \
	${THREADX_DIR}/common/src/txe_thread_entry_exit_notify.c \
	${THREADX_DIR}/common/src/txe_thread_info_get.c \
	${THREADX_DIR}/common/src/txe_thread_preemption_change.c \
	${THREADX_DIR}/common/src/txe_thread_priority_change.c \
	${THREADX_DIR}/common/src/txe_thread_relinquish.c \
	${THREADX_DIR}/common/src/txe_thread_reset.c \
	${THREADX_DIR}/common/src/txe_thread_resume.c \
	${THREADX_DIR}/common/src/txe_thread_suspend.c \
	${THREADX_DIR}/common/src/txe_thread_terminate.c \
	${THREADX_DIR}/common/src/txe_thread_time_slice_change.c \
	${THREADX_DIR}/common/src/txe_thread_wait_abort.c \
	${THREADX_DIR}/common/src/txe_timer_activate.c \
	${THREADX_DIR}/common/src/txe_timer_change.c \
	${THREADX_DIR}/common/src/txe_timer_create.c \
	${THREADX_DIR}/common/src/txe_timer_deactivate.c \
	${THREADX_DIR}/common/src/txe_timer_delete.c \
	${THREADX_DIR}/common/src/txe_timer_info_get.c \

APP_DEFS += -DTX_INCLUDE_USER_DEFINE_FILE

ifeq (STM32F722, $(strip $(MCU)))
INCS += $(THREADX_DIR)/ports/cortex_m7/gnu/inc
SRCS += $(THREADX_DIR)/ports/cortex_m7/gnu/src/tx_thread_context_restore.S
SRCS += $(THREADX_DIR)/ports/cortex_m7/gnu/src/tx_thread_context_save.S
SRCS += $(THREADX_DIR)/ports/cortex_m7/gnu/src/tx_thread_interrupt_control.S
SRCS += $(THREADX_DIR)/ports/cortex_m7/gnu/src/tx_thread_interrupt_disable.S
SRCS += $(THREADX_DIR)/ports/cortex_m7/gnu/src/tx_thread_interrupt_restore.S
SRCS += $(THREADX_DIR)/ports/cortex_m7/gnu/src/tx_thread_schedule.S
SRCS += $(THREADX_DIR)/ports/cortex_m7/gnu/src/tx_thread_stack_build.S
SRCS += $(THREADX_DIR)/ports/cortex_m7/gnu/src/tx_thread_system_return.S
SRCS += $(THREADX_DIR)/ports/cortex_m7/gnu/src/tx_timer_interrupt.S
endif