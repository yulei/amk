
#include "matrix.h"
#include "action.h"

void suspend_idle(uint8_t time) {}

void suspend_power_down(void) { }
__attribute__ ((weak)) void matrix_power_up(void) {}
__attribute__ ((weak)) void matrix_power_down(void) {}

bool suspend_wakeup_condition(void)
{
    matrix_power_up();
    matrix_scan();
    matrix_power_down();
    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        if (matrix_get_row(r)) return true;
    }
    return false;
}

// run immediately after wakeup
void suspend_wakeup_init(void)
{
    // clear keyboard state
    matrix_clear();
    clear_keyboard();
}