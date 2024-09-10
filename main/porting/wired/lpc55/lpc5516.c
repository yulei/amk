/**
 * @file at32f405.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "amk_hal.h"
#include "cm_misc.h"
#include "wait.h"
#include "usb_common.h"
#include "amk_printf.h"

#include "tusb.h"

/*******************************************************************************
 * Definitions for BOARD_BootClockPLL150M configuration
 ******************************************************************************/
#define BOARD_BOOTCLOCKPLL150M_CORE_CLOCK         150000000U  /*!< Core clock frequency: 150000000Hz */


/* Clock outputs (values are in Hz): */
#define BOARD_BOOTCLOCKPLL150M_ASYNCADC_CLOCK         0UL            /* Clock consumers of ASYNCADC_clock output : ADC0 */
#define BOARD_BOOTCLOCKPLL150M_CAN_CLOCK              0UL            /* Clock consumers of CAN_clock output : CAN0 */
#define BOARD_BOOTCLOCKPLL150M_CLKOUT_CLOCK           0UL            /* Clock consumers of CLKOUT_clock output : N/A */
#define BOARD_BOOTCLOCKPLL150M_CTIMER0_CLOCK          0UL            /* Clock consumers of CTIMER0_clock output : CTIMER0 */
#define BOARD_BOOTCLOCKPLL150M_CTIMER1_CLOCK          0UL            /* Clock consumers of CTIMER1_clock output : CTIMER1 */
#define BOARD_BOOTCLOCKPLL150M_CTIMER2_CLOCK          0UL            /* Clock consumers of CTIMER2_clock output : CTIMER2 */
#define BOARD_BOOTCLOCKPLL150M_CTIMER3_CLOCK          0UL            /* Clock consumers of CTIMER3_clock output : CTIMER3 */
#define BOARD_BOOTCLOCKPLL150M_CTIMER4_CLOCK          0UL            /* Clock consumers of CTIMER4_clock output : CTIMER4 */
#define BOARD_BOOTCLOCKPLL150M_FRO_12MHZ_CLOCK        12000000UL     /* Clock consumers of FRO_12MHz_clock output : ANACTRL */
#define BOARD_BOOTCLOCKPLL150M_FRO_1MHZ_CLOCK         0UL            /* Clock consumers of FRO_1MHz_clock output : N/A */
#define BOARD_BOOTCLOCKPLL150M_FXCOM0_CLOCK           0UL            /* Clock consumers of FXCOM0_clock output : FLEXCOMM0 */
#define BOARD_BOOTCLOCKPLL150M_FXCOM1_CLOCK           0UL            /* Clock consumers of FXCOM1_clock output : FLEXCOMM1 */
#define BOARD_BOOTCLOCKPLL150M_FXCOM2_CLOCK           0UL            /* Clock consumers of FXCOM2_clock output : FLEXCOMM2 */
#define BOARD_BOOTCLOCKPLL150M_FXCOM3_CLOCK           0UL            /* Clock consumers of FXCOM3_clock output : FLEXCOMM3 */
#define BOARD_BOOTCLOCKPLL150M_FXCOM4_CLOCK           0UL            /* Clock consumers of FXCOM4_clock output : FLEXCOMM4 */
#define BOARD_BOOTCLOCKPLL150M_FXCOM5_CLOCK           0UL            /* Clock consumers of FXCOM5_clock output : FLEXCOMM5 */
#define BOARD_BOOTCLOCKPLL150M_FXCOM6_CLOCK           0UL            /* Clock consumers of FXCOM6_clock output : FLEXCOMM6 */
#define BOARD_BOOTCLOCKPLL150M_FXCOM7_CLOCK           0UL            /* Clock consumers of FXCOM7_clock output : FLEXCOMM7 */
#define BOARD_BOOTCLOCKPLL150M_HSLSPI_CLOCK           0UL            /* Clock consumers of HSLSPI_clock output : FLEXCOMM8 */
#define BOARD_BOOTCLOCKPLL150M_HSUSB1_32K_CLOCK       32768UL        /* Clock consumers of HSUSB1_32K_clock output : USBPHY */
#define BOARD_BOOTCLOCKPLL150M_MCLK_CLOCK             0UL            /* Clock consumers of MCLK_clock output : N/A */
#define BOARD_BOOTCLOCKPLL150M_OSC32KHZ_CLOCK         32768UL        /* Clock consumers of OSC32KHZ_clock output : FLEXCOMM0, FLEXCOMM1, FLEXCOMM2, FLEXCOMM3, FLEXCOMM4, FLEXCOMM5, FLEXCOMM6, FLEXCOMM7, FLEXCOMM8, USBHSD, USBHSH */
#define BOARD_BOOTCLOCKPLL150M_OSTIMER_CLOCK          0UL            /* Clock consumers of OSTIMER_clock output : OSTIMER */
#define BOARD_BOOTCLOCKPLL150M_PLUCLKIN_CLOCK         0UL            /* Clock consumers of PLUCLKIN_clock output : PLU */
#define BOARD_BOOTCLOCKPLL150M_PLU_GLITCH_12MHZ_CLOCK 0UL            /* Clock consumers of PLU_GLITCH_12MHz_clock output : PLU */
#define BOARD_BOOTCLOCKPLL150M_PLU_GLITCH_1MHZ_CLOCK  0UL            /* Clock consumers of PLU_GLITCH_1MHz_clock output : PLU */
#define BOARD_BOOTCLOCKPLL150M_RTC1HZ_CLOCK           0UL            /* Clock consumers of RTC1HZ_clock output : N/A */
#define BOARD_BOOTCLOCKPLL150M_RTC1KHZ_CLOCK          0UL            /* Clock consumers of RTC1KHZ_clock output : N/A */
#define BOARD_BOOTCLOCKPLL150M_SCT_CLOCK              0UL            /* Clock consumers of SCT_clock output : SCT0 */
#define BOARD_BOOTCLOCKPLL150M_SYSTICK0_CLOCK         0UL            /* Clock consumers of SYSTICK0_clock output : N/A */
#define BOARD_BOOTCLOCKPLL150M_SYSTEM_CLOCK           150000000UL    /* Clock consumers of System_clock output : ADC0, ANACTRL, CAN0, CRC_ENGINE, CTIMER0, CTIMER1, CTIMER2, CTIMER3, CTIMER4, DMA0, DMA1, FLASH, FLEXCOMM0, FLEXCOMM1, FLEXCOMM2, FLEXCOMM3, FLEXCOMM4, FLEXCOMM5, FLEXCOMM6, FLEXCOMM7, FLEXCOMM8, GINT0, GINT1, GPIO, INPUTMUX, IOCON, MRT0, OSTIMER, PINT, PLU, SCT0, SECGPIO, SECPINT, SWD, SYSCTL, USB0, USBFSH, USBHSD, USBHSH, USBPHY, UTICK0, WWDT */
#define BOARD_BOOTCLOCKPLL150M_TRACE_CLOCK            0UL            /* Clock consumers of TRACE_clock output : SWD */
#define BOARD_BOOTCLOCKPLL150M_USB0_CLOCK             0UL            /* Clock consumers of USB0_clock output : USB0, USBFSH */
#define BOARD_BOOTCLOCKPLL150M_USB1_PHY_CLOCK         16000000UL     /* Clock consumers of USB1_PHY_clock output : USBHSD, USBHSH, USBPHY */
#define BOARD_BOOTCLOCKPLL150M_UTICK_CLOCK            0UL            /* Clock consumers of UTICK_clock output : UTICK0 */
#define BOARD_BOOTCLOCKPLL150M_WDT_CLOCK              0UL            /* Clock consumers of WDT_clock output : WWDT */

void system_clock_init(void)
{
    /*!< Set up the clock sources */
    /*!< Configure FRO192M */
    POWER_DisablePD(kPDRUNCFG_PD_FRO192M);               /*!< Ensure FRO is on  */
    CLOCK_SetupFROClocking(12000000U);                   /*!< Set up FRO to the 12 MHz, just for sure */
    CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);                /*!< Switch to FRO 12MHz first to ensure we can change the clock setting */

    /*!< Configure XTAL32M */
    POWER_DisablePD(kPDRUNCFG_PD_XTAL32M);                        /* Ensure XTAL32M is powered */
    POWER_DisablePD(kPDRUNCFG_PD_LDOXO32M);                       /* Ensure XTAL32M is powered */
    CLOCK_SetupExtClocking(16000000U);                            /* Enable clk_in clock */
    SYSCON->CLOCK_CTRL |= SYSCON_CLOCK_CTRL_CLKIN_ENA_MASK;       /* Enable clk_in from XTAL32M clock  */
    ANACTRL->XO32M_CTRL |= ANACTRL_XO32M_CTRL_ENABLE_SYSTEM_CLK_OUT_MASK;    /* Enable High speed Crystal oscillator output to system  */
    ANACTRL->XO32M_CTRL |= ANACTRL_XO32M_CTRL_ENABLE_PLL_USB_OUT_MASK;       /* Enable High speed Crystal oscillator output to HS USB  */

    /*!< Configure RTC OSC */
    POWER_EnablePD(kPDRUNCFG_PD_XTAL32K);                /*!< Powered down the XTAL 32 kHz RTC oscillator */
    POWER_DisablePD(kPDRUNCFG_PD_FRO32K);                /*!< Powered the FRO 32 kHz RTC oscillator */
    CLOCK_AttachClk(kFRO32K_to_OSC32K);                  /*!< Switch OSC32K to FRO32K */
    CLOCK_EnableClock(kCLOCK_Rtc);                       /*!< Enable the RTC peripheral clock */
    RTC->CTRL &= ~RTC_CTRL_SWRESET_MASK;                 /*!< Make sure the reset bit is cleared */

    POWER_SetVoltageForFreq(150000000U);                  /*!< Set voltage for the one of the fastest clock outputs: System clock output */
    CLOCK_SetFLASHAccessCyclesForFreq(150000000U);          /*!< Set FLASH wait states for core */

    /*!< Set up PLL */
    CLOCK_AttachClk(kEXT_CLK_to_PLL0);                    /*!< Switch PLL0CLKSEL to EXT_CLK */
    POWER_DisablePD(kPDRUNCFG_PD_PLL0);                  /* Ensure PLL is on  */
    POWER_DisablePD(kPDRUNCFG_PD_PLL0_SSCG);
    const pll_setup_t pll0Setup = {
        .pllctrl = SYSCON_PLL0CTRL_CLKEN_MASK | SYSCON_PLL0CTRL_SELI(53U) | SYSCON_PLL0CTRL_SELP(31U),
        .pllndec = SYSCON_PLL0NDEC_NDIV(8U),
        .pllpdec = SYSCON_PLL0PDEC_PDIV(1U),
        .pllsscg = {0x0U,(SYSCON_PLL0SSCG1_MDIV_EXT(150U) | SYSCON_PLL0SSCG1_SEL_EXT_MASK)},
        .pllRate = 150000000U,
        .flags =  PLL_SETUPFLAG_WAITLOCK
    };
    CLOCK_SetPLL0Freq(&pll0Setup);                       /*!< Configure PLL0 to the desired values */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U, false);         /*!< Set AHBCLKDIV divider to value 1 */

    /*!< Set up clock selectors - Attach clocks to the peripheries */
    CLOCK_AttachClk(kPLL0_to_MAIN_CLK);                 /*!< Switch MAIN_CLK to PLL0 */
    CLOCK_AttachClk(kOSC32K_to_CLK32K);                 /*!< Switch CLK32K to OSC32K */
    CLOCK_AttachClk(kOSC32K_to_OSTIMER);                 /*!< Switch OSTIMER to OSC32K */

    /*!< Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKPLL150M_CORE_CLOCK;
}

#include "usb_phy.h"
#define USB_DEVICE_CONTROLLER_ID kUSB_ControllerLpcIp3511Hs0

static void usb_custom_init(void)
{
    /* enable usb1 host clock */
    CLOCK_EnableClock(kCLOCK_Usbh1);

    /* Put PHY powerdown under software control */
    *((uint32_t *)(USBHSH_BASE + 0x50)) = USBHSH_PORTMODE_SW_PDCOM_MASK;
    /*According to reference mannual, device mode setting has to be set by access usb host register */
    *((uint32_t *)(USBHSH_BASE + 0x50)) |= USBHSH_PORTMODE_DEV_ENABLE_MASK;

    /* enable usb1 host clock */
    CLOCK_DisableClock(kCLOCK_Usbh1);

#if CHIRP_ISSUE_WORKAROUND_NEEDED
    USB_TimerInit(0, 1000U);
#endif

    POWER_DisablePD(kPDRUNCFG_PD_LDOUSBHS); /*!< Ensure xtal32k is on  */

    POWER_DisablePD(kPDRUNCFG_PD_USB1_PHY); /* Turn on power for USB PHY */
    uint32_t delay = 100000;
    while (delay --)
    {
        __asm("nop");
    }

    CLOCK_EnableClock(kCLOCK_Usbd1);
    CLOCK_EnableClock(kCLOCK_UsbRam1);
    CLOCK_EnableClock(kCLOCK_Usb1Clk);

	uint32_t pllDivSelValue = 0;
	uint32_t inputFrequency = CLOCK_GetUsb1ClkFreq();
    switch(CLOCK_GetUsb1ClkFreq()) {
    case 32000000:
    	pllDivSelValue = 0;
    	break;
    case 30000000:
    	pllDivSelValue = 1;
    	break;
    case 24000000:
    	pllDivSelValue = 2;
    	break;
    case 20000000:
    	pllDivSelValue = 4;
    	break;
    case 19200000:
    	pllDivSelValue = 5;
    	break;
    case 16000000:
    	pllDivSelValue = 6;
    	break;
    case 12000000:
    	pllDivSelValue = 7;
    	break;
    default:
    	amk_printf("Unsupported input frequency (%lu) for USB PHY.\r\n", inputFrequency);
    }

    USBPHY->CTRL_CLR    = USBPHY_CTRL_SFTRST_MASK;
    USBPHY->PLL_SIC     = (USBPHY->PLL_SIC & ~USBPHY_PLL_SIC_PLL_DIV_SEL(0x7)) | USBPHY_PLL_SIC_PLL_DIV_SEL(pllDivSelValue);
    USBPHY->PLL_SIC_SET = USBPHY_PLL_SIC_SET_PLL_REG_ENABLE_MASK;
    USBPHY->PLL_SIC_CLR = (1 << 16);
    USBPHY->PLL_SIC_SET = USBPHY_PLL_SIC_SET_PLL_POWER_MASK;
    USBPHY->PLL_SIC_SET = USBPHY_PLL_SIC_SET_PLL_EN_USB_CLKS_MASK;

    USBPHY->CTRL_CLR = USBPHY_CTRL_CLR_CLKGATE_MASK;
    USBPHY->PWD_SET  = 0x0;

    uint32_t not_used = 0;
    USB_EhciPhyInit(USB_DEVICE_CONTROLLER_ID, not_used, NULL);

#if defined(FSL_FEATURE_USBHSD_USB_RAM) && (FSL_FEATURE_USBHSD_USB_RAM)
    for (int i = 0; i < FSL_FEATURE_USBHSD_USB_RAM; i++)
    {
        ((uint8_t *)FSL_FEATURE_USBHSD_USB_RAM_BASE_ADDRESS)[i] = 0x00U;
    }
#endif
}

void custom_board_init(void)
{
    // init gpio clocks
    CLOCK_EnableClock(kCLOCK_Iocon);
    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);

    usb_custom_init();
}

void custom_board_task(void)
{}

void USB1_IRQHandler(void)
{
    tud_int_handler(1);
}