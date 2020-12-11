/**
 * ssd1357.c
 */
#include "ssd1357.h"
#include "spi.h"
#include "wait.h"

#define BRIGHTNESS  0x0F
#define COL_BEGIN   0x20
#define COL_END     0x5f
#define ROW_BEGIN   0x00
#define ROW_END     0x3f

enum{
    CMD_SetColumnAddress = 0x15,
    CMD_SetRowAddress = 0x75,
    CMD_WriteRAM = 0x5C,
    CMD_ReadRAM,
    CMD_SetRemapColorDepth = 0xA0,
    CMD_SetDisplayStartLine,
    CMD_SetDisplayOffset,
    CMD_SDM_ALL_OFF = 0xA4,
    CMD_SDM_ALL_ON,
    CMD_SDM_RESET,
    CMD_SDM_INVERSE,
    CMD_SetSleepMode_ON = 0xAE,
    CMD_SetSleepMode_OFF,
    CMD_SetResetPrechargePeriod = 0xB1,
    CMD_SetClkDiv = 0xB3,
    CMD_SetSecondPrechargePeriod = 0xB6,
    CMD_MLUTGrayscale = 0xB8,
    CMD_UseDefMLUT,
    CMD_SetPrechargeVoltage = 0xBB,
    CMD_SetILUTColorA,
    CMD_SetILUTColorC,
    CMD_SetVCOMH,
    CMD_SetContrastCurrentABC = 0xC1,
    CMD_SetMasterContrastCurrent = 0xC7,
    CMD_SetMuxRatio = 0xCA,
    CMD_NOP = 0xE3,
    CMD_SetCommandLock = 0xFD, 
    CMD_Setup_Scrolling = 0x96,
    SCROLL_STOP = 0x9E,
    SCROLL_START
};

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Gray Scale Table Setting (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
static const uint8_t gamma[] = {
    0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13,
    0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F, 0x21, 0x23, 0x25,
    0x27, 0x2A, 0x2D, 0x30, 0x33, 0x36, 0x39, 0x3C, 0x3F,
    0x42, 0x45, 0x48, 0x4C, 0x50, 0x54, 0x58, 0x5C, 0x60,
    0x64, 0x68, 0x6C, 0x70, 0x74, 0x78, 0x7D, 0x82, 0x87,
    0x8C, 0x91, 0x96, 0x9B, 0xA0, 0xA5, 0xAA, 0xAF, 0xB4,
};

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Gray Scale Pulse Width for Color A
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
/*static const uint8_t gamma_colorA[] = {
    0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
    0x12, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F,
    0x21, 0x23, 0x25, 0x27, 0x2A, 0x2D, 0xB4,
};*/

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Gray Scale Pulse Width for Color C
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
/*static const uint8_t gamma_colorC[] = {
    0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
    0x12, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F,
    0x21, 0x23, 0x25, 0x27, 0x2A, 0x2D, 0xB4,
};*/

static spi_handle_t spi;

static void write_command(ssd1357_t *driver, uint8_t command)
{
    gpio_write_pin(driver->cs, 0);
    gpio_write_pin(driver->dc, 0);

    spi_send(spi, &command, sizeof(command));

    gpio_write_pin(driver->cs, 1);
    gpio_write_pin(driver->dc, 1);
}

static void write_data(ssd1357_t *driver, uint8_t data)
{
    gpio_write_pin(driver->cs, 0);
    gpio_write_pin(driver->dc, 1);

    spi_send(spi, &data, sizeof(data));

    gpio_write_pin(driver->cs, 1);
    gpio_write_pin(driver->dc, 1);
}

static void write_data_buffer(ssd1357_t *driver, const void *data, size_t size)
{
    gpio_write_pin(driver->cs, 0);
    gpio_write_pin(driver->dc, 1);

    spi_send(spi, data, size);

    gpio_write_pin(driver->cs, 1);
    gpio_write_pin(driver->dc, 1);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Instruction Setting
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
static void set_column_address(ssd1357_t *driver, uint8_t begin, uint8_t end)
{
    write_command(driver, CMD_SetColumnAddress); // Set Column Address
    write_data(driver, begin);   // Default => 0x00
    write_data(driver, end);     // Default => 0x7F
}

static void set_row_address(ssd1357_t *driver, uint8_t begin, uint8_t end)
{
    write_command(driver, CMD_SetRowAddress); // Set Row Address
    write_data(driver, begin);   // Default => 0x00
    write_data(driver, end);     // Default => 0x7F
}

static void set_write_ram(ssd1357_t *driver)
{
    write_command(driver, CMD_WriteRAM); // Enable MCU to Write into RAM
}

/*
static void set_read_ram(ssd1357_t *driver)
{
    write_command(driver, CMD_ReadRAM); // Enable MCU to Read from RAM
}
*/

static void set_remap_format(ssd1357_t *driver, uint8_t format)
{
    write_command(driver, CMD_SetRemapColorDepth);// A[0]=0b, Horizontal address increment [reset]
    write_data(driver, format); // DA[1]=0b, Column address 0 is mapped to SEG0 [reset]
                                // A[2]=0b, Color sequence: A  B C [reset]
                                // A[3]=0b, Reserved [reset]
                                // A[4]=0b, Scan from COM0 to COM[N ¡V1] [reset]
                                // A[5]=1b, Enable COM Split Odd Even [reset]
                                // A[7:6] Set Color Depth 65k color [reset]
    write_data(driver, 0x00);   // Default => 0x01 (Disable Dual COM Mode)
}

static void set_start_line(ssd1357_t *driver, uint8_t line)
{
    write_command(driver, CMD_SetDisplayStartLine); // Set Vertical Scroll by RAM
    write_data(driver, line);    // Default => 0x00
}

static void set_display_offset(ssd1357_t *driver, uint8_t offset)
{
    write_command(driver, CMD_SetDisplayOffset); // Set Display Offset
    write_data(driver, offset);  // Default => 0x00
}

static void set_display_mode(ssd1357_t *driver, uint8_t mode)
{
    write_command(driver, mode); // Set Display Mode
                                 // Default => 0xA4
                                 // 0xA4 => Entire Display Off, All Pixels Turn Off
                                 // 0xA5 => Entire Display On, All Pixels Turn On at GS Level 15
                                 // 0xA6 => Normal Display
                                 // 0xA7 => Inverse Display
}

static void set_display_on_off(ssd1357_t *driver, uint8_t onoff)
{
    write_command(driver, onoff); // Set Display On/Off
                                  // Default => 0xAE
                                  // 0xAE (0x00) => Display Off (Sleep Mode On)
                                  // 0xAF (0x01) => Display On (Sleep Mode Off)
}

static void set_phase_length(ssd1357_t *driver, uint8_t phase)
{
    write_command(driver, CMD_SetResetPrechargePeriod); // Phase 1 (Reset) & Phase 2 (Pre-Charge) Period Adjustment
    write_data(driver, phase);   // Default => 0x84 (16 Display Clocks [Phase 2] / 8 Display Clocks [Phase 1])
                                 // D[3:0] => Phase 1 Period in 2~30 Display Clocks
                                 // D[7:4] => Phase 2 Period in 2~30 Display Clocks
}

static void set_display_clock(ssd1357_t *driver, uint8_t clock)
{
    write_command(driver, CMD_SetClkDiv); // Set Display Clock Divider / Oscillator Frequency
    write_data(driver, clock);   // Default => 0x20
                                 // A[3:0] => Display Clock Divider
                                 // A[7:4] => Oscillator Frequency
}

static void set_precharge_period(ssd1357_t *driver, uint8_t period)
{
    write_command(driver, CMD_SetSecondPrechargePeriod); // Set Second Pre-Charge Period
    write_data(driver, period);  // Default => 0x08 (8 Display Clocks)
}

static void set_gray_scale_table(ssd1357_t *driver)
{
    write_command(driver, CMD_MLUTGrayscale); // Set Gray Scale Table

    write_data_buffer(driver, gamma, sizeof(gamma));
}
/*
static void set_linear_gray_scale_table(ssd1357_t *driver)
{
    write_command(driver, 0xB9); // Set Default Linear Gray Scale Table
}*/

static void set_precharge_voltage(ssd1357_t *driver, unsigned char d)
{
    write_command(driver, CMD_SetPrechargeVoltage); // Set Pre-Charge Voltage Level
    write_data(driver, d);       // Default => 0x1E (0.50*VCC)
}

/*
static void set_gray_pulse_width_color(ssd1357_t *driver, uint8_t command, const void *data, size_t size)
{
    write_command(driver, command);
    write_data_buffer(driver, data, size);
}

static void set_gray_scale_pulse_width_colora(ssd1357_t *driver, void)
{
    set_gray_pulse_width_color(driver, 0xBC, gamma_colorA, sizeof(gamma_colorA));
}

static void set_gray_scale_pulse_width_colorc(ssd1357_t *driver, void)
{
    set_gray_pulse_width_color(driver, 0xBD, gamma_colorC, sizeof(gamma_colorC));
}
*/

static void set_vcomh(ssd1357_t *driver, uint8_t level)
{
    write_command(driver, CMD_SetVCOMH); // Set COM Deselect Voltage Level
    write_data(driver, level);   // Default => 0x05 (0.82*VCC)
}

static void set_contrast_current(ssd1357_t *driver, uint8_t color_a, uint8_t color_b, uint8_t color_c)
{
    write_command(driver, CMD_SetContrastCurrentABC); // Set Contrast Current
    write_data(driver, color_a); // Contrast Value Color A Default => 0x7F
    write_data(driver, color_b); // Contrast Value Color B Default => 0x7F
    write_data(driver, color_c); // Contrast Value Color C Default => 0x7F
}

static void set_master_current(ssd1357_t *driver, uint8_t current)
{
    write_command(driver, CMD_SetMasterContrastCurrent); // Master Contrast Current Control
    write_data(driver, current); // Default => 0x0f (Maximum)
}

static void set_multiplex_ratio(ssd1357_t *driver, uint8_t ratio)
{
    write_command(driver, CMD_SetMuxRatio); // Set Multiplex Ratio
    write_data(driver, ratio);   // Default => 0x7F (1/128 Duty)
}

/*static void set_nop(void)
{
    write_command(driver, 0xE3); // Command for No Operation
}*/

static void set_command_lock(ssd1357_t *driver, uint8_t lock)
{
    write_command(driver, CMD_SetCommandLock); // Set Command Lock
    write_data(driver, lock);    // Default => 0x12
                         // 0x12 => Driver IC interface is unlocked from entering command.
                         // 0x16 => All Commands are locked except 0xFD.
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
static void fill_screen(ssd1357_t *driver, uint16_t color)
{
    set_column_address(driver, COL_BEGIN, COL_END);
    set_row_address(driver, ROW_BEGIN, ROW_END);
    set_write_ram(driver);

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            write_data_buffer(driver, &color, sizeof(color));
        }
    }
}
/*
static void flush_screen(ssd1357_t *driver, const void *data, size_t size)
{
    set_column_address(driver, 0x20, 0x5F);
    set_row_address(driver, 0x00, 0x3F);
    set_write_ram(driver);
    write_data_buffer(driver, data, size);
}*/

void ssd1357_init(ssd1357_t *driver)
{
    spi = spi_init();
    gpio_write_pin(driver->reset, 0);
    wait_ms(5);
    gpio_write_pin(driver->reset, 1);
    wait_ms(200);

    set_command_lock(driver, 0x12);                 // Unlock Basic Commands (0x12/0x16)
    set_display_on_off(driver, 0xAE);               // Display Off (0xAE/0xAF)
    set_display_clock(driver, 0xB0);                // Set Clock as 80 Frames/Sec
    set_multiplex_ratio(driver, 0x3F);              // 1/64 Duty (0x0F~0x3F)
    set_display_offset(driver, 0x40);               // Shift Mapping RAM Counter (0x00~0x3F)
    set_start_line(driver, 0x00);                   // Set Mapping RAM Display Start Line (0x00~0x7F)
    set_remap_format(driver, 0x76);                 // Set Horizontal Address Increment
                                                    // Column address 0 is mapped to SEG0
                                                    // Color sequence: A „³ B „³ C
                                                    // Reserved
                                                    // Scan from COM[N-1] to COM0
                                                    // Enable COM Split Odd Even
                                                    // Pseudo 262k color, 16-bit format 2
    set_contrast_current(driver, 0x88, 0x32, 0x88); // Set Segment Output Current
    set_master_current(driver, BRIGHTNESS);         // Set Scale Factor of Segment Output Current Control
    set_phase_length(driver, 0x32);                 // Set Phase 1 as 17 Clocks & Phase 2 as 14 Clocks
    set_precharge_period(driver, 0x01);             // Set Second Pre-Charge Period as 8 Clocks
    set_gray_scale_table(driver);                   // Set Pulse Width for Gray Scale Table
    set_precharge_voltage(driver, 0x17);            // Set Pre-Charge Voltage Level as 0.4069*VCC
    set_vcomh(driver, 0x05);                        // Set Common Pins Deselect Voltage Level as 0.86*VCC
    set_column_address(driver, COL_BEGIN, COL_END); // Set Column Address
    set_row_address(driver, ROW_BEGIN, ROW_END);    // Set Row Address
    set_display_mode(driver, 0xA6);                 // Normal Display Mode (0xA4/0xA5/0xA6/0xA7)
    fill_screen(driver, 0x0000);                    // Clear Screen
    set_display_on_off(driver, 0xAF);               // Display On (0xAE/0xAF)
    // need to power up screen here
    // wait_ms(200);
}

void ssd1357_fill_rect(ssd1357_t* driver, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void *data, size_t size)
{
    set_column_address(driver, COL_BEGIN+y, COL_BEGIN+y+height-1);
    set_row_address(driver, ROW_BEGIN+x, ROW_BEGIN+x+width-1);
    set_write_ram(driver);
    //write_data_buffer(driver, data, size);
    uint16_t* color = (uint16_t*)data;
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            write_data_buffer(driver, color, sizeof(*color));
            ++color;
        }
    }
}

void ssd1357_fill(ssd1357_t* driver, const void* data)
{
    uint16_t *color = (uint16_t*)data;
    set_column_address(driver, COL_BEGIN, COL_END);
    set_row_address(driver, ROW_BEGIN, ROW_END);
    set_write_ram(driver);

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            write_data_buffer(driver, color, sizeof(*color));
            color++;
        }
    }
}
void ssd1331_uninit(void)
{
}