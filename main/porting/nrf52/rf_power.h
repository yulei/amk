/**
 * rf_power.h
 *      power saving process
 */

#include <stdint.h>

#ifndef RF_POWER_MODULE_MAX
#define RF_POWER_MODULE_MAX     8       /**< maximum module can be registered */
#endif

/**@brief callback function for preparing sleep */

typedef void (*rf_peripheral_sleep_t)(void* context);

/**@brief Function for initializing the module
 */
void rf_power_init(void);

/**@brief Function for register the prepare sleep callback
 * 
 * @param[in]   sleep       the call back function for preparing sleep
 * @param[in]   context     user supplied parameters
 * 
 * @return 0 while success
 */
uint32_t rf_power_register(rf_peripheral_sleep_t sleep, void* context);

/**@brief Call this function beftore the keyboard go to the sleep state
 */
void rf_power_prepare_sleep(void);

/**@brief Uninitialized this module
 */
void rf_power_uninit(void);
