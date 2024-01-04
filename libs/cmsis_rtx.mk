CMSIS_DIR := $(LIB_DIR)/cmsis_6/CMSIS
CMSIS_RTX_DIR := $(LIB_DIR)/cmsis_rtx

INCS += \
	$(CMSIS_DIR)/RTOS2/Include \
	$(CMSIS_RTX_DIR)/Config \
	$(CMSIS_RTX_DIR)/Include \

SRCS += \
	$(CMSIS_RTX_DIR)/Source/rtx_delay.c \
	$(CMSIS_RTX_DIR)/Source/rtx_evflags.c \
	$(CMSIS_RTX_DIR)/Source/rtx_evr.c \
	$(CMSIS_RTX_DIR)/Source/rtx_kernel.c \
	$(CMSIS_RTX_DIR)/Source/rtx_lib.c \
	$(CMSIS_RTX_DIR)/Source/rtx_memory.c \
	$(CMSIS_RTX_DIR)/Source/rtx_mempool.c \
	$(CMSIS_RTX_DIR)/Source/rtx_msgqueue.c \
	$(CMSIS_RTX_DIR)/Source/rtx_mutex.c \
	$(CMSIS_RTX_DIR)/Source/rtx_semaphore.c \
	$(CMSIS_RTX_DIR)/Source/rtx_system.c \
	$(CMSIS_RTX_DIR)/Source/rtx_thread.c \
	$(CMSIS_RTX_DIR)/Source/rtx_timer.c \
	$(CMSIS_RTX_DIR)/Config/RTX_Config.c \
	$(CMSIS_DIR)/RTOS2/Source/os_systick.c \

ifeq (CORTEX_M4, $(strip $(MCU_ARCH)))
	SRCS += $(CMSIS_RTX_DIR)/Source/GCC/irq_armv7m.S
endif

ifeq (CORTEX_M7, $(strip $(MCU_ARCH)))
	SRCS += $(CMSIS_RTX_DIR)/Source/GCC/irq_armv7m.S
endif

ifeq (CORTEX_M33, $(strip $(MCU_ARCH)))
	SRCS += $(CMSIS_RTX_DIR)/Source/GCC/irq_armv8mbl.S
endif
