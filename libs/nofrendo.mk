NOFRENDO_DIR := $(LIB_DIR)/nofrendo/src

SRCS += \
	$(NOFRENDO_DIR)/bitmap.c \
	$(NOFRENDO_DIR)/config.c \
	$(NOFRENDO_DIR)/dis6502.c \
	$(NOFRENDO_DIR)/event.c \
	$(NOFRENDO_DIR)/gui_elem.c \
	$(NOFRENDO_DIR)/gui.c \
	$(NOFRENDO_DIR)/log.c \
	$(NOFRENDO_DIR)/mapvrc.c \
	$(NOFRENDO_DIR)/memguard.c \
	$(NOFRENDO_DIR)/mmc5_snd.c \
	$(NOFRENDO_DIR)/mmclist.c \
	$(NOFRENDO_DIR)/nes_apu.c \
	$(NOFRENDO_DIR)/nes_mmc.c \
	$(NOFRENDO_DIR)/nes_pal.c \
	$(NOFRENDO_DIR)/nes_ppu.c \
	$(NOFRENDO_DIR)/nes_rom.c \
	$(NOFRENDO_DIR)/nes.c \
	$(NOFRENDO_DIR)/nes6502.c \
	$(NOFRENDO_DIR)/nesinput.c \
	$(NOFRENDO_DIR)/nesstate.c \
	$(NOFRENDO_DIR)/nofrendo.c \
	$(NOFRENDO_DIR)/pcx.c \
	$(NOFRENDO_DIR)/vid_drv.c \
	$(NOFRENDO_DIR)/vrcvisnd.c \

SRCS += $(wildcard $(NOFRENDO_DIR)/mappers/*.c)

INCS += \
	$(NOFRENDO_DIR) \

APP_DEFS += -DNOFRENDO_ENABLE
