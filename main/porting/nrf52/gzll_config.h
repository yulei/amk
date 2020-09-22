/**
 * @file gzll_config.h
 */

#pragma once

#include "common_config.h"

#define GZLL_MAX_TX_ATTEMPTS        100  /**< Maximum number of Transmit attempts.*/
#define GZLL_DUMMY_TYPE             0xFF /**< test payload.*/
#define GZLL_PAYLOAD_SIZE           8    /**< Size of the payload to send over Gazell.*/
#define GZLL_PIPE_TO_HOST           0    /**< Pipe number. */

#define GZLL_SLEEP_SCAN             500
#define GZLL_KEEPALIVE_INTERVAL     10  