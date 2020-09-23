/**
 * @file gzll_config.h
 */

#pragma once

#include "common_config.h"

#define GZLL_MAX_TX_ATTEMPTS        100             /**< Maximum number of Transmit attempts.*/
#define GZLL_KEEPALIVE_TYPE         0xFF            /**< keep alive packet.*/
#define GZLL_PAYLOAD_SIZE           8               /**< Size of the payload to send over Gazell.*/
#define GZLL_PIPE_TO_HOST           0               /**< Pipe number. */
#define GZLL_BASE_ADDRESS_0         0x4D584D58      /**< base address 0. */
#define GZLL_BASE_ADDRESS_1         0x54265426      /**< base address 1. */

#define GZLL_SLEEP_SCAN             500             /**< idle scan count for turnning sleep. */
#define GZLL_KEEPALIVE_INTERVAL     10              /**< keep alive packet sending interval. */