/**
 * @file lpm_main.c
 */

#include "generic_hal.h"
#include "board.h"
#include "usb_interface.h"
#include "rgb_led.h"
#include "quantum.h"
#include "amk_driver.h"
#include "amk_gpio.h"
#include "amk_utils.h"
#include "amk_printf.h"

#ifdef RF_ENABLE
#include "rf_driver.h"
#endif

#ifdef MSC_ENABLE
#include "mscusb.h"
#endif

extern void system_init(void);
extern void custom_board_init(void);
extern void custom_board_task(void);

static bool is_matrix_on(void)
{
	bool on = false;
	matrix_row_t zero = 0;
	for (uint8_t i= 0; i < MATRIX_ROWS; i++) {

        on |= matrix_get_row(i) ^ zero;
    }

	return on;
}

static uint32_t counter = 0;

int main(int argc, char ** argv)
{
    system_init();
    amk_printf("custom_board_init\n");
    custom_board_init();

    amk_printf("usb_init\n");
    usb_init();

    amk_driver_init();
    amk_printf("amk_init\n");

    for (;;) {
        usb_task();

        amk_driver_task();
        if (!is_matrix_on()) {
            counter++;
        } else {
            counter = 0;
        }


#if defined(MSC_ENABLE)
        msc_task();
#endif

#ifdef AUDIO_ENABLE
        audio_task();
#endif

#ifdef SCREEN_ENABLE
        render_task();
#endif

#ifdef RGB_ENABLE
        rgb_led_task();
#endif

#ifdef RF_ENABLE
        rf_driver_task();
#endif

        custom_board_task();
        if (0){//(counter > 0x00FF) {
            HAL_SuspendTick();

            amk_printf("enter sleep\n");
            //gpio_write_pin(CAPS_LED_PIN, 1);

            HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

            HAL_ResumeTick();
            amk_printf("exit sleep\n");
            counter = 0;
            //gpio_write_pin(CAPS_LED_PIN, 0);
        }
    }

    return 0;
}
