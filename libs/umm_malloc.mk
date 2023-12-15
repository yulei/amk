
UMM_MALLOC_DIR := $(LIB_DIR)/umm_malloc/src

SRCS += $(UMM_MALLOC_DIR)/umm_malloc.c

SRCS += $(UMM_MALLOC_DIR)/umm_info.c
SRCS += $(UMM_MALLOC_DIR)/umm_integrity.c
SRCS += $(UMM_MALLOC_DIR)/umm_poison.c

INCS += $(UMM_MALLOC_DIR)
