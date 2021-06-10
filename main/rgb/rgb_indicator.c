/**
 * @file rgb_indicator.h
 */

#include <stdint.h>
#include "aw9106b.h"
#include "gpio_pin.h"
#include "wait.h"

void rgb_indicator_init(void)
{
#ifdef AW9106B_SHDN_PIN
    gpio_set_output_pushpull(AW9106B_SHDN_PIN);
    gpio_write_pin(AW9106B_SHDN_PIN, 1);
    wait_ms(1);
#endif

#ifdef AW9106B_ADDR1
    aw9106b_init(AW9106B_ADDR1, 0);
#endif

#ifdef AW9106B_ADDR2
    aw9106b_init(AW9106B_ADDR2, 1);
#endif
    aw9106b_set_color_all(0, 0, 0);
}

void rgb_indicator_set(uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    aw9106b_set_color(index, red, green, blue);
}

void rgb_indicator_task(void)
{
#ifdef AW9106B_ADDR1
    aw9106b_update_buffers(AW9106B_ADDR1, 0);
#endif

#ifdef AW9106B_ADDR2
    aw9106b_update_buffers(AW9106B_ADDR2, 1);
#endif
}

void rgb_indicator_uninit(void)
{
#ifdef AW9106B_SHDN_PIN
    gpio_set_output_pushpull(AW9106B_SHDN_PIN);
    gpio_write_pin(AW9106B_SHDN_PIN, 0);
#endif
}