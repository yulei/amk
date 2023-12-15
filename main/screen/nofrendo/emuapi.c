
#include  <errno.h>
#include  <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO

//#include "umm_malloc.h"
#include "static_memory.h"
#include "arcada_def.h"
#include "nes_display.h"
#include "nes_emu.h"
#include "ff.h"
#include "amk_printf.h"

//#include <Adafruit_Arcada.h>
//extern Adafruit_Arcada arcada;

#include "emuapi.h"
#include "nesstate.h"

//extern Display_DMA tft;
extern char rom_filename_path[512];
static FIL file;
volatile bool vbl=true;
int skip=0;

#if EMU_SCALEDOWN == 1
  // Packed 16-big RGB palette (big-endian order)
  unsigned short palette16[PALETTE_SIZE];
#elif EMU_SCALEDOWN == 2
  // Bizarro palette for 2x2 downsampling.
  // Red, green and blue values go into a 32-bit unsigned type with gaps.
  // Bits 29:22 are red (8 bits), bits 18:11 are blue (8 bits), and bits
  // 8:1 are green (8 bits). RBG order and gap positions are intentional:
  // 00RRRRRRRR000BBBBBBBB00GGGGGGGG0
  uint32_t palette32[PALETTE_SIZE];
  // Later, bitwise shenanigans are used to accumulate 2x2 pixel colors
  // and shift/mask these into a 16-bit result.
#else
  #error("Only scale 1 or 2 supported")
#endif


#define LOADMENU_SELECTIONS 3
#define LOADMENU_LOADSAVED  0
#define LOADMENU_LOADCLEAN  1
#define LOADMENU_DELETEFILE 2
const char *loadmenu_strings[LOADMENU_SELECTIONS] = {"Load Saved Game", "Start New Game", "Delete Save File"};

void emu_Halt(const char * error_msg) {
  amk_printf(error_msg);
  //tft.stop();
  nes_display.fill_screen(ARCADA_BLACK);
  //arcada.haltBox(error_msg);
}

void emu_printf(const char format[], ...)
{
    char s[255];

    va_list ap;
    va_start(ap, format);
    vsnprintf_(s, sizeof(s), format, ap);
    amk_printf(s);
    va_end(ap);
}

void emu_Init(char* rom)
 {
  nes_Start(rom);
  nes_Init();
}

void emu_Step(uint32_t frame)
{
  nes_Step();
}

void * emu_Malloc(int size)
{
  void * retval =  static_memory_alloc(size);
  if (!retval) {
    char error_msg[255];
    sprintf(error_msg, "emu_Malloc: failed to allocate %d bytes", size);
    emu_Halt(error_msg);
  }
  else {
    amk_printf("emu_Malloc: successfully allocated %d bytes\n", size);
  }

  return retval;
}

void emu_Free(void * pt)
{
  emu_printf("Freeing memory");
  static_memory_free(pt);
}

int emu_FileOpen(char * filename)
{
  int retval = 0;
  //Serial.print("FileOpen: "); Serial.print(filename);

  //if (file = arcada.open(filename, O_READ)) {
  if (FR_OK == f_open(&file, filename, FA_READ)) {
    //Serial.println("...Success!");
    retval = 1;
  } else {
    char error_msg[255];
    sprintf(error_msg, "emu_FileOpen: failed to open file %s", filename);
    emu_Halt(error_msg);
  }
  return (retval);
}

uint8_t *emu_LoadROM(char *filename) {
  //Serial.print("LoadROM: "); Serial.print(filename);
  //tft.stop();
  nes_display.fill_screen(ARCADA_CYAN);
  //arcada.display->fillScreen(ARCADA_CYAN);
  //arcada.infoBox("Loading ROM into FLASH memory...", 0);
//  uint8_t *romdata = arcada.writeFileToFlash(filename, DEFAULT_FLASH_ADDRESS);
  uint8_t *romdata = 0;//arcada.writeFileToFlash(filename);
  //Serial.printf(" into address $%08x", (uint32_t)romdata);
  if ((uint32_t)romdata == 0) {
    emu_Halt("Unable to load file into FLASH, maybe too large?");
  }
  nes_display.fill_screen(ARCADA_RED);
  //arcada.display->fillScreen(ARCADA_BLACK);
  //tft.refresh();
  return romdata;
}

int emu_FileRead(uint8_t* buf, int size)
{
  UINT retval = 0;
  if (FR_OK != f_read(&file, buf, size, &retval)) {
    emu_Halt("FileRead failed");
  } else {
    emu_printf("File readed=%d\n", retval);
  }

  return (retval);
}

unsigned char emu_FileGetc(void) {
  unsigned char c;
  UINT retval = 0;
  if (FR_OK != f_read(&file, &c, 1, &retval)) {
    emu_Halt("emu_FileGetc failed");
  }
  return c;
}


void emu_FileClose(void)
{
  f_close(&file);
}

int emu_FileSize(char * filename)
{
  int filesize = 0;
  emu_printf("FileSize...");
  emu_printf(filename);
  FIL tmp;

  if (FR_OK == f_open(&tmp, filename, FA_READ)) {
    filesize = f_size(&tmp);
    emu_printf("filesize is...%d\n", filesize);
    f_close(&tmp);
  }

  return (filesize);
}

int emu_FileSeek(int pos)
{
  f_lseek(&file, pos);
  return pos;
}

int emu_LoadFile(char * filename, char * buf, int numbytes) {
  int filesize = 0;

  emu_printf("LoadFile %s...", filename);

  if (FR_OK == f_open(&file, filename, FA_READ)) {
    filesize = f_size(&file);
    emu_printf("Size %d\n", filesize);
    if (numbytes >= filesize) {
      UINT readed = 0;
      if (FR_OK != f_read(&file, buf, filesize, &readed) || readed != filesize) {
        emu_Halt("File read failed");
      } else {
        emu_printf("File load success\n");
      }
    }
    f_close(&file);
  }

  return (filesize);
}

int emu_LoadFileSeek(char * filename, char * buf, int numbytes, int pos)
{
  int filesize = 0;
  emu_printf("LoadFileSeek...%s\n", filename);

  if (FR_OK == f_open(&file, filename, FA_READ)) {
    if (FR_OK != f_lseek(&file, pos)) {
      emu_Halt("File seek failed");
    } else {
      UINT readed = 0;
      if (FR_OK != f_read(&file, buf, filesize, &readed) || readed != filesize) {
        emu_Halt("File read failed");
      } else {
        filesize = readed;
      }
    }
    f_close(&file);
  }
  return filesize;
}



void emu_SaveState() {
#if defined(USE_SAVEFILES)
  char filename[512];
  strcpy(filename, rom_filename_path);
  char *fp = filename + strlen(filename);
  fp[0] = '.';
  fp[1] = 's';
  fp[2] = 'a';
  fp[3] = 'v';
  fp[4] = 0;
  if (arcada.exists(filename)) {
    arcada.remove(filename);
  }
  //Serial.print("Saving state to file:");
  //Serial.println(filename);
  state_save(filename);
#else
  //tft.stop();
  //delay(50);
  //arcada.infoBox("Save state not supported!");
  //arcada.display->fillScreen(ARCADA_BLACK);
  //tft.refresh();

#endif
}

void emu_LoadState(char skipMenu) {
#if defined(USE_SAVEFILES)
  char filename[512];
  strncpy(filename, rom_filename_path, 500);
  char *fp = filename + strlen(filename);
  fp[0] = '.';
  fp[1] = 's';
  fp[2] = 'a';
  fp[3] = 'v';
  fp[4] = 0;
  if (arcada.exists(filename)) {
    if (skipMenu) {
      state_load(filename);
      return;
    }
    //Serial.println("Found savefile");
    tft.stop();
    delay(50);
    uint8_t selected = 0;
    arcada.display->fillScreen(ARCADA_BLUE);
    selected = arcada.menu(loadmenu_strings, LOADMENU_SELECTIONS, ARCADA_WHITE, ARCADA_BLACK);
    //Serial.printf("Selected %d\n", selected);
    if (selected ==  LOADMENU_LOADSAVED) {
      //Serial.print("Loading state from file:");
      //Serial.println(filename);
      state_load(filename);      
    }
    if (selected == LOADMENU_DELETEFILE) {
      arcada.remove(filename);
    }
    // either way, restart the display
    arcada.display->fillScreen(ARCADA_BLACK);
    tft.refresh();
  }
#else
  //tft.stop();
  //delay(50);
  //arcada.infoBox("Save state not supported!");
  //arcada.display->fillScreen(ARCADA_BLACK);
  //tft.refresh();
#endif
}

//static uint16_t bLastState;
//uint16_t button_CurState;
uint16_t emu_DebounceLocalKeys(void) {
 // button_CurState = arcada.readButtons();

//  uint16_t bClick = button_CurState & ~bLastState;
//  bLastState = button_CurState;

  return 0;//bClick;
}

#include "keyboard.h"
#include "matrix.h"

extern keypos_t arcada_keys[ARCADA_BUTTON_COUNT];

uint32_t emu_ReadKeys(void)
{
  uint32_t button = 0;
  for (int i = 0; i < ARCADA_BUTTON_COUNT; i++) {
    int row = arcada_keys[i].row;
    int col = arcada_keys[i].col;
    matrix_row_t data = matrix_get_row(row);
    if (data&(1<<col)) {
      button |= (1<<i);
    }
  }
  return button;
}



void emu_SetPaletteEntry(unsigned char r, unsigned char g, unsigned char b, int index)
{
  if (index<PALETTE_SIZE) {
    //Serial.println("%d: %d %d %d\n", index, r,g,b);
#if EMU_SCALEDOWN == 1
    palette16[index] = __builtin_bswap16(RGBVAL16(r,g,b));
#elif EMU_SCALEDOWN == 2
    // 00RRRRRRRR000BBBBBBBB00GGGGGGGG0
    palette32[index] =
      ((uint32_t)r << 22) | ((uint32_t)b << 11) | ((uint32_t)g << 1);
#endif
  }
}

void emu_DrawVsync(void)
{
  volatile bool vb=vbl;
  skip += 1;
  skip &= VID_FRAME_SKIP;

  while (vbl==vb) {break;};
}

bool frametoggle;
void emu_DrawLine(unsigned char * VBuf, int width, int height, int line) 
{
  if (line == 0) {
    //digitalWrite(FRAME_LED, frametoggle);
    frametoggle = !frametoggle;
  }
#if EMU_SCALEDOWN == 1
  nes_display.write_line(width, 1, line, VBuf, palette16);
#elif EMU_SCALEDOWN == 2
  nes_display.write_line(width, 1, line, VBuf, palette32);
#endif
}  

#if 0
// NOT CURRENTLY BEING USED IN NOFRENDO.
// If it does get used, may need to handle 2x2 palette methodology.
// See also writeScreen() in display_dma.cpp.
void emu_DrawScreen(unsigned char * VBuf, int width, int height, int stride) 
{
  if (skip==0) {
    tft.writeScreen(width,height-TFT_VBUFFER_YCROP,stride, VBuf+(TFT_VBUFFER_YCROP/2)*stride, palette16);
  }
}
#endif

int emu_FrameSkip(void)
{
  return skip;
}

void *emu_LineBuffer(int line)
{
  return (void*)nes_display.get_line_buffer(line);
}


void emu_sndInit()
{
}

#include <string.h>
char *emu_Strdup(const char *src)
{
  size_t size = strlen(src);
  char* dst = static_memory_alloc(size+1);
  if (!dst) return NULL;
  strcpy(dst, src);
  return dst;
}