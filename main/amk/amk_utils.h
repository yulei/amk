/**
 * @file amk_utils.h
 * 
 */

#pragma once

#define AMK_CONTAINER_OF(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#define AMK_ARRAY_SIZE(x)                   (sizeof(x) / sizeof((x)[0]))
#define AMK_STRING(x)                       #x
#define AMK_XSTRING(x)                      AMK_STRING(x)
#define AMK_STRCAT(a, b)                     a##b
#define AMK_XSTRCAT(a, b)                    AMK_STRCAT(a, b)