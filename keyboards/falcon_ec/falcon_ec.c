/**
 * @file falcon_ec.c
 */

#include "falcon_ec.h"
#include "amk_gpio.h"
#include "amk_printf.h"
#include "ec_matrix.h"
#include "wait.h"

ec_matrix_t ec_matrix = {
    {
        { 
            {78,1733,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {75,1637,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {77,1616,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {80,1561,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {77,1455,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {83,1258,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {83,1619,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {82,1555,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {76,1526,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {78,1520,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {78,1547,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {77,1624,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {76,1571,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {77,1463,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {80,1561,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
        },
        {
            {77,1626,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {78,1513,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {79,1614,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {98,1643,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {102,1771,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {78,1614,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {120,1600,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {128,1718,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {80,1580,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {79,1732,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {78,1694,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {78,1662,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {78,1602,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {78,1613,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {77,1592,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
        },
        {
            {76,1612,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {84,1584,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {122,1485,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {85,1652,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {80,1562,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {80,1656,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {82,1619,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {81,1723,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {79,1633,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {90,1732,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {98,1759,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {80,1509,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {79,1587,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {90,1635,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {80,1683,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
        },
        {
            {77,1641,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {78,1692,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {80,1570,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {79,1502,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {88,1635,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {80,1645,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {82,270,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {108,1612,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {80,1678,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {98,1681,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {79,1645,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {80,1713,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {80,1652,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {143,1944,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
            {79,1580,EC_AUTO_MIN, EC_AUTO_MAX, 0, STROKE_DEFAULT},
        },
    },
    //EC_DEFAULT_MODE,
    EC_AUTO_MODE,
    //EC_CALIBRATE_MODE,
};

void matrix_init_kb(void)
{
    #ifdef CAPS_LED_PIN
    gpio_set_output_pushpull(CAPS_LED_PIN);
    gpio_write_pin(CAPS_LED_PIN, 0);
    #endif

    #ifdef RGB_EN_PIN
    gpio_set_output_pushpull(RGB_EN_PIN);
    gpio_write_pin(RGB_EN_PIN, 1);
    #endif

    //power on switch board
    #ifdef POWER_PIN
    gpio_set_output_pushpull(POWER_PIN);
    gpio_write_pin(POWER_PIN, 1);

    wait_ms(100);
    #endif

    // initialize opamp and capacitor discharge
    gpio_set_output_pushpull(OPA_EN_PIN);
    gpio_write_pin(OPA_EN_PIN, 1);
    gpio_set_output_pushpull(DISCHARGE_PIN);
    gpio_write_pin(DISCHARGE_PIN, 1);
}

#ifdef RGB_ENABLE
#include "rgb_driver.h"
#include "rgb_linear.h"
rgb_led_t g_rgb_leds[RGB_LED_NUM] = {
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 2, 2, 2},
    {0, 3, 3, 3},
};


rgb_device_t g_rgb_devices[RGB_DEVICE_NUM] = {
    {RGB_DRIVER_WS2812, 0, 0, 0, 4},
};

rgb_param_t g_rgb_linear_params[RGB_SEGMENT_NUM] = {
    {0, 0, 4},
};
#endif
