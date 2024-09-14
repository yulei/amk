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