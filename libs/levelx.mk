
LEVELX_DIR := $(LIB_DIR)/rtos/levelx

INCS += $(LEVELX_DIR)/common/inc

LIB_SRCS += \
	${LEVELX_DIR}/src/fx_nand_flash_simulated_driver.c \
	${LEVELX_DIR}/src/fx_nor_flash_simulator_driver.c \
	${LEVELX_DIR}/src/lx_nand_flash_256byte_ecc_check.c \
	${LEVELX_DIR}/src/lx_nand_flash_256byte_ecc_compute.c \
	${LEVELX_DIR}/src/lx_nand_flash_block_full_update.c \
	${LEVELX_DIR}/src/lx_nand_flash_block_obsoleted_check.c \
	${LEVELX_DIR}/src/lx_nand_flash_block_reclaim.c \
	${LEVELX_DIR}/src/lx_nand_flash_close.c \
	${LEVELX_DIR}/src/lx_nand_flash_defragment.c \
	${LEVELX_DIR}/src/lx_nand_flash_driver_block_erase.c \
	${LEVELX_DIR}/src/lx_nand_flash_driver_block_erased_verify.c \
	${LEVELX_DIR}/src/lx_nand_flash_driver_block_status_get.c \
	${LEVELX_DIR}/src/lx_nand_flash_driver_block_status_set.c \
	${LEVELX_DIR}/src/lx_nand_flash_driver_extra_bytes_get.c \
	${LEVELX_DIR}/src/lx_nand_flash_driver_extra_bytes_set.c \
	${LEVELX_DIR}/src/lx_nand_flash_driver_page_erased_verify.c \
	${LEVELX_DIR}/src/lx_nand_flash_driver_read.c \
	${LEVELX_DIR}/src/lx_nand_flash_driver_write.c \
	${LEVELX_DIR}/src/lx_nand_flash_extended_cache_enable.c \
	${LEVELX_DIR}/src/lx_nand_flash_initialize.c \
	${LEVELX_DIR}/src/lx_nand_flash_logical_sector_find.c \
	${LEVELX_DIR}/src/lx_nand_flash_next_block_to_erase_find.c \
	${LEVELX_DIR}/src/lx_nand_flash_open.c \
	${LEVELX_DIR}/src/lx_nand_flash_page_ecc_check.c \
	${LEVELX_DIR}/src/lx_nand_flash_page_ecc_compute.c \
	${LEVELX_DIR}/src/lx_nand_flash_partial_defragment.c \
	${LEVELX_DIR}/src/lx_nand_flash_physical_page_allocate.c \
	${LEVELX_DIR}/src/lx_nand_flash_sector_mapping_cache_invalidate.c \
	${LEVELX_DIR}/src/lx_nand_flash_sector_read.c \
	${LEVELX_DIR}/src/lx_nand_flash_sector_release.c \
	${LEVELX_DIR}/src/lx_nand_flash_sector_write.c \
	${LEVELX_DIR}/src/lx_nand_flash_simulator.c \
	${LEVELX_DIR}/src/lx_nand_flash_system_error.c \
	${LEVELX_DIR}/src/lx_nor_flash_block_reclaim.c \
	${LEVELX_DIR}/src/lx_nor_flash_close.c \
	${LEVELX_DIR}/src/lx_nor_flash_defragment.c \
	${LEVELX_DIR}/src/lx_nor_flash_driver_block_erase.c \
	${LEVELX_DIR}/src/lx_nor_flash_driver_read.c \
	${LEVELX_DIR}/src/lx_nor_flash_driver_write.c \
	${LEVELX_DIR}/src/lx_nor_flash_extended_cache_enable.c \
	${LEVELX_DIR}/src/lx_nor_flash_initialize.c \
	${LEVELX_DIR}/src/lx_nor_flash_logical_sector_find.c \
	${LEVELX_DIR}/src/lx_nor_flash_next_block_to_erase_find.c \
	${LEVELX_DIR}/src/lx_nor_flash_open.c \
	${LEVELX_DIR}/src/lx_nor_flash_partial_defragment.c \
	${LEVELX_DIR}/src/lx_nor_flash_physical_sector_allocate.c \
	${LEVELX_DIR}/src/lx_nor_flash_sector_mapping_cache_invalidate.c \
	${LEVELX_DIR}/src/lx_nor_flash_sector_read.c \
	${LEVELX_DIR}/src/lx_nor_flash_sector_release.c \
	${LEVELX_DIR}/src/lx_nor_flash_sector_write.c \
	${LEVELX_DIR}/src/lx_nor_flash_simulator.c \
	${LEVELX_DIR}/src/lx_nor_flash_system_error.c \

APP_DEFS += -DLX_INCLUDE_USER_DEFINE_FILE
