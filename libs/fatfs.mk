FATFFS_DIR := $(LIB_DIR)/fatfs

SRCS += \
	$(FATFFS_DIR)/source/ff.c \
	$(FATFFS_DIR)/source/ffsystem.c \
	$(FATFFS_DIR)/porting/diskio_flash.c \

INCS += \
	$(FATFFS_DIR)/source \
	$(FATFFS_DIR)/porting \
