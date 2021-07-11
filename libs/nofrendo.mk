NOFRENDO_DIR := $(LIB_DIR)/nofrendo

SRCS += \
	$(NOFRENDO_DIR)/src/bitmap.c \
	$(NOFRENDO_DIR)/src/config.c \
	$(NOFRENDO_DIR)/src/event.c \
	$(NOFRENDO_DIR)/src/gui_elem.c \
	$(NOFRENDO_DIR)/src/gui.c \
	$(NOFRENDO_DIR)/src/intro.c \
	$(NOFRENDO_DIR)/src/log.c \
	$(NOFRENDO_DIR)/src/memguard.c \
	$(NOFRENDO_DIR)/src/nofrendo.c \
	$(NOFRENDO_DIR)/src/pcx.c \
	$(NOFRENDO_DIR)/src/vid_drv.c \
	$(NOFRENDO_DIR)/src/cpu/dis6502.c \
	$(NOFRENDO_DIR)/src/cpu/nes6502.c \
	$(NOFRENDO_DIR)/src/libsnss/libsnss.c \
	$(NOFRENDO_DIR)/src/nes/mmclist.c \
	$(NOFRENDO_DIR)/src/nes/nes_mmc.c \
	$(NOFRENDO_DIR)/src/nes/nes_pal.c \
	$(NOFRENDO_DIR)/src/nes/nes_ppu.c \
	$(NOFRENDO_DIR)/src/nes/nes_rom.c \
	$(NOFRENDO_DIR)/src/nes/nes.c \
	$(NOFRENDO_DIR)/src/nes/nesinput.c \
	$(NOFRENDO_DIR)/src/nes/nesstate.c \
	$(NOFRENDO_DIR)/src/sndhrdw/fds_snd.c \
	$(NOFRENDO_DIR)/src/sndhrdw/mmc5_snd.c \
	$(NOFRENDO_DIR)/src/sndhrdw/nes_apu.c \
	$(NOFRENDO_DIR)/src/sndhrdw/vrcvisnd.c \
	$(wildcard $(NOFRENDO_DIR)/mappers/*.c)

INCS += \
	$(NOFRENDO_DIR)/src/ \
	$(NOFRENDO_DIR)/src/cpu \
	$(NOFRENDO_DIR)/src/libsnss \
	$(NOFRENDO_DIR)/src/mappers \
	$(NOFRENDO_DIR)/src/nes \
	$(NOFRENDO_DIR)/src/sndhrdw \


APP_DEFS += -DNOFRENDO_ENABLE

#umm_malloc
UMALLOC_DIR := $(LIB_DIR)/umm_malloc

SRCS += \
	$(UMALLOC_DIR)/src/umm_info.c \
	$(UMALLOC_DIR)/src/umm_integrity.c \
	$(UMALLOC_DIR)/src/umm_malloc.c \
	$(UMALLOC_DIR)/src/umm_poison.c \

INCS += \
	$(UMALLOC_DIR)/src \
	$(UMALLOC_DIR)/test/support


