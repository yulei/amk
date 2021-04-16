/**
 * @file atsamd.c
 */

#include "gpio_pin.h"
#include "usb_descriptors.h"

struct flash_descriptor FLASH_0;
struct usart_sync_descriptor USART_0;

static void FLASH_0_CLOCK_init(void)
{
    _pm_enable_bus_clock(PM_BUS_APBB, NVMCTRL);
}

static void FLASH_0_init(void)
{
    FLASH_0_CLOCK_init();
    flash_init(&FLASH_0, NVMCTRL);
}

static void USART_0_PORT_init(void)
{
    gpio_set_pin_function(A4, PINMUX_PA04D_SERCOM0_PAD0);
    gpio_set_pin_function(A5, PINMUX_PA05D_SERCOM0_PAD1);
}

static void USART_0_CLOCK_init(void)
{
    _pm_enable_bus_clock(PM_BUS_APBC, SERCOM0);
    _gclk_enable_channel(SERCOM0_GCLK_ID_CORE, CONF_GCLK_SERCOM0_CORE_SRC);
}

static void USART_0_init(void)
{
    USART_0_CLOCK_init();
    usart_sync_init(&USART_0, SERCOM0, (void *)NULL);
    USART_0_PORT_init();
}

static void USB_DEVICE_0_PORT_init(void)
{
    gpio_set_pin_direction(A24, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(A24, false);
    gpio_set_pin_pull_mode(A24, GPIO_PULL_OFF); 
    gpio_set_pin_function(A24, PINMUX_PA24G_USB_DM);

    gpio_set_pin_direction(A25, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(A25, false);
    gpio_set_pin_pull_mode(A25, GPIO_PULL_OFF);
    gpio_set_pin_function(A25, PINMUX_PA25G_USB_DP);
}

/* The USB module requires a GCLK_USB of 48 MHz ~ 0.25% clock
 * for low speed and full speed operation. */
#if (CONF_GCLK_USB_FREQUENCY > (48000000 + 48000000 / 400)) || (CONF_GCLK_USB_FREQUENCY < (48000000 - 48000000 / 400))
#warning USB clock should be 48MHz ~ 0.25% clock, check your configuration!
#endif

void USB_DEVICE_0_CLOCK_init(void)
{
    _pm_enable_bus_clock(PM_BUS_APBB, USB);
    _pm_enable_bus_clock(PM_BUS_AHB, USB);
    _gclk_enable_channel(USB_GCLK_ID, CONF_GCLK_USB_SRC);
}

void USB_DEVICE_0_init(void)
{
    USB_DEVICE_0_CLOCK_init();
    USB_DEVICE_0_PORT_init();
}

void custom_board_init(void)
{
	FLASH_0_init();
	USART_0_init();
	USB_DEVICE_0_init();
}

void custom_board_task(void)
{
}

void system_init(void)
{
    init_mcu();
    SysTick_Config(CONF_CPU_FREQUENCY/1000);
}

static uint32_t systick_count = 0;  // systick count
uint32_t systick_get_tick(void)
{
    return systick_count;
}

// IRQ handler
void SysTick_Handler(void)
{
    systick_count++;
}

void USB_Handler(void)
{
    tud_int_handler(0);
}