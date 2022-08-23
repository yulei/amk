/**
 * @file amk_profile.h
 * @author astro
 * @brief code profiling with DWT and output through RTT
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdint.h>

#ifdef DWT_DELAY
#pragma message "DWT was used for delay, thus can't use with PROFILE"
#else
uint32_t AMK_PROFILE_BEGIN(const char* profile_region);
void AMK_PROFILE_SAMPLE(const char* profile_sample, uint32_t region);
void AMK_PROFILE_END(uint32_t region);
#endif
