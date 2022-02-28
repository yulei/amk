/**
 * @file amk_utils.h
 * 
 */

#pragma once

#define AMK_CONTAINER_OF(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#define AMK_ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
