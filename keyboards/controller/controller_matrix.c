/**
 * controller_matrix.c
 */

#include <stdbool.h>

#include "app_timer.h"
#include "gpio_pin.h"

#include "matrix.h"
#include "wait.h"
#include "timer.h"
#include "matrix_scan.h"

// matrix power saving
static uint32_t matrix_last_modified = 0;
static bool matrix_powered = false;

// matrix state buffer(1:on, 0:off)
static matrix_row_t *matrix;
static matrix_row_t *matrix_prev;
static matrix_row_t _matrix0[MATRIX_ROWS];
static matrix_row_t _matrix1[MATRIX_ROWS];

static void init_pins(void);
static void hhkb_power_enable(void);
static void hhkb_power_disable(void);
static void hhkb_key_enable(void);
static void hhkb_key_disable(void);
static void hhkb_key_select(uint8_t row, uint8_t col);
static void hhkb_key_prev_on(void);
static void hhkb_key_prev_off(void);
static bool hhkb_key_state(void);

void matrix_init(void)
{
    init_pins();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) _matrix0[i] = 0x00;
    for (uint8_t i=0; i < MATRIX_ROWS; i++) _matrix1[i] = 0x00;
    matrix = _matrix0;
    matrix_prev = _matrix1;
}

uint8_t matrix_scan(void)
{
    uint8_t *tmp;

    tmp = matrix_prev;
    matrix_prev = matrix;
    matrix = tmp;

    // power on
    hhkb_power_enable();

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            hhkb_key_select(row, col);
            wait_us(5);

            // Not sure this is needed. This just emulates HHKB controller's behaviour.
            if (matrix_prev[row] & (1<<col)) {
                hhkb_key_prev_on();
            }
            wait_us(10);

            // NOTE: KEY_STATE is valid only in 20us after KEY_ENABLE.
            // If V-USB interrupts in this section we could lose 40us or so
            // and would read invalid value from KEY_STATE.
            uint32_t last = app_timer_cnt_get();

            hhkb_key_enable();

            // Wait for KEY_STATE outputs its value.
            // 1us was ok on one HHKB, but not worked on another.
            // no   wait doesn't work on Teensy++ with pro(1us works)
            // no   wait does    work on tmk PCB(8MHz) with pro2
            // 1us  wait does    work on both of above
            // 1us  wait doesn't work on tmk(16MHz)
            // 5us  wait does    work on tmk(16MHz)
            // 5us  wait does    work on tmk(16MHz/2)
            // 5us  wait does    work on tmk(8MHz)
            // 10us wait does    work on Teensy++ with pro
            // 10us wait does    work on 328p+iwrap with pro
            // 10us wait doesn't work on tmk PCB(8MHz) with pro2(very lagged scan)
            wait_us(5);

            if (hhkb_key_state()) {
                matrix[row] &= ~(1<<col);
            } else {
                matrix[row] |= (1<<col);
            }

            // Ignore if this code region execution time elapses more than 20us.
            // MEMO: 20[us] * (TIMER_RAW_FREQ / 1000000)[count per us]
            // MEMO: then change above using this rule: a/(b/c) = a*1/(b/c) = a*(c/b)
            uint32_t current = app_timer_cnt_get();
            if (TIMER_DIFF_32(current, last) > 20*(APP_TIMER_TICKS(1)*1000)) {
                matrix[row] = matrix_prev[row];
            }

            wait_us(5);
            hhkb_key_prev_off();
            hhkb_key_disable();
            
            // NOTE: KEY_STATE keep its state in 20us after KEY_ENABLE.
            // This takes 25us or more to make sure KEY_STATE returns to idle state.
#ifdef HHKB_JP
            // Looks like JP needs faster scan due to its twice larger matrix
            // or it can drop keys in fast key typing
            wait_us(30);
#else
            wait_us(75);
#endif
        }
        if (matrix[row] ^ matrix_prev[row]) matrix_last_modified = timer_read32();
    }
    // power off
    /*if (KEY_POWER_STATE() &&
            (USB_DeviceState == DEVICE_STATE_Suspended ||
             USB_DeviceState == DEVICE_STATE_Unattached ) &&
            timer_elapsed32(matrix_last_modified) > MATRIX_POWER_SAVE) {
        KEY_POWER_OFF();
        suspend_power_down();
    }*/
    return 1;
}

matrix_row_t matrix_get_row(uint8_t row) { return matrix[row]; }

static void init_pins(void)
{
    gpio_set_input_pullup(HHKB_KEY_IN);

    gpio_set_output_pushpull(HHKB_POWER_EN);
    gpio_set_output_pushpull(HHKB_HYS);
    gpio_set_output_pushpull(HHKB_HC_A);
    gpio_set_output_pushpull(HHKB_HC_B);
    gpio_set_output_pushpull(HHKB_HC_C);
    gpio_set_output_pushpull(HHKB_LS_A);
    gpio_set_output_pushpull(HHKB_LS_B);
    gpio_set_output_pushpull(HHKB_LS_C);
    gpio_set_output_pushpull(HHKB_LS_D);

    hhkb_key_disable();
    hhkb_key_prev_off();
    hhkb_power_disable();
}

static void hhkb_power_enable(void)
{
    if (matrix_powered)
        return;
    gpio_write_pin(HHKB_POWER_EN, 1);
    wait_ms(5);
    matrix_powered = true;
}


static void hhkb_power_disable(void)
{
    if (!matrix_powered)
        return;
    gpio_write_pin(HHKB_POWER_EN, 0);
    matrix_powered = false;
}

static void hhkb_key_enable(void)
{
    gpio_write_pin(HHKB_LS_D, 0);
}

static void hhkb_key_disable(void)
{
    gpio_write_pin(HHKB_LS_D, 1);
}

static void hhkb_key_prev_on(void)
{
    gpio_write_pin(HHKB_HYS, 1);
}

static void hhkb_key_prev_off(void)
{
    gpio_write_pin(HHKB_HYS, 0);
}

static bool hhkb_key_state(void)
{
    return gpio_read_pin(HHKB_KEY_IN) == 0 ? false : true;
}

static void hhkb_key_select(uint8_t row, uint8_t col)
{
    gpio_write_pin(HHKB_HC_A, (row>>0) & 0x01);
    gpio_write_pin(HHKB_HC_B, (row>>1) & 0x01);
    gpio_write_pin(HHKB_HC_C, (row>>2) & 0x01);

    gpio_write_pin(HHKB_LS_A, (col>>0) & 0x01);
    gpio_write_pin(HHKB_LS_B, (col>>1) & 0x01);
    gpio_write_pin(HHKB_LS_C, (col>>2) & 0x01);
}

void matrix_prepare_sleep(void)
{
    hhkb_power_disable();
}

bool matrix_keys_off(void)
{
    for (int i = 0; i < MATRIX_ROWS; i++) {
        if (matrix[i] != 0) {
            return false;
        }
    }
    return true;
}
