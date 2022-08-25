/**
 * @file amk_profile.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "generic_hal.h"
#include "amk_profile.h"
#include "amk_printf.h"
#include "amk_timer.h"

#if defined(AMK_PROFILING) && !defined(DWT_DELAY)

#define AMK_SAMPLE_NUM  16
#define AMK_REGION_NUM  32

typedef struct {
    uint32_t ticks;
    const char* name;
} amk_sample_t;

typedef struct {
    uint32_t id;
    uint32_t start;
    uint32_t end;
    uint32_t count;
    uint32_t index;
    amk_sample_t samples[AMK_SAMPLE_NUM];
    const char* name;
} amk_region_t;

typedef struct {
    amk_region_t regions[AMK_REGION_NUM];
    uint32_t index;
} amk_profile_t;

static amk_profile_t amk_profile = {.index = 0};
static bool profile_dwt_inited = false;

uint32_t AMK_PROFILE_BEGIN(const char* profile_region)
{
    // start DWT
    if (!profile_dwt_inited) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
        profile_dwt_inited = true;
    }

    // initialize current region
    amk_region_t *region = &amk_profile.regions[amk_profile.index++];
    memset(region, 0, sizeof(amk_region_t));
    region->id = amk_profile.index - 1;
    region->name = profile_region;
    region->start = DWT->CYCCNT;
    return region->id;
}

void AMK_PROFILE_SAMPLE(const char* profile_sample, uint32_t region_id)
{
    #if 0
    amk_sample_t *sample = NULL;
    for (int i = 0;i < i < AMK_REGION_NUM; i++) {
        if (amk_profile.regions[i].id == region) {
            amk_region_t *region = &amk_profile.regions[i];
            sample = &region->samples[region->index++];
            break;
        }
    }
    #endif
    amk_region_t *region = &amk_profile.regions[region_id];
    amk_sample_t *sample = &region->samples[region->index ++];

    sample->name = profile_sample;
    sample->ticks = DWT->CYCCNT;
}

static void profile_dump_region(amk_region_t *region)
{
    uint32_t ticks_per_us = SystemCoreClock/1000000;

    amk_printf("\n-----region: %s-------start: %d\n", region->name, region->start/ticks_per_us);
    for (int i = 0; i < region->index; i++) {
        amk_sample_t *sample = &region->samples[i];
        amk_printf("...sample: %s......sampled: %d.....delay: %d\n", sample->name, sample->ticks/ticks_per_us, (TIMER_DIFF_32(sample->ticks, region->start))/ticks_per_us);
    }
    amk_printf("-----region: %s-------end-----\n", region->name);
}

void AMK_PROFILE_END(uint32_t region)
{
    for (int i = 0; i < amk_profile.index; i++) {
        profile_dump_region(&amk_profile.regions[i]);
    }

    memset(&amk_profile, 0, sizeof(amk_profile_t));
}

#else
uint32_t AMK_PROFILE_BEGIN(const char* profile_region)
{
    return 0;
}

void AMK_PROFILE_SAMPLE(const char* profile_sample, uint32_t region)
{
}

void AMK_PROFILE_END(uint32_t region)
{
}

#endif