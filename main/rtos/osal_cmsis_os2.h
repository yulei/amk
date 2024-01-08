/**
 * @file osal_threadx.h
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "cmsis_os2.h"

//--------------------------------------------------------------------+
// TASK API
//--------------------------------------------------------------------+
static inline void osal_task_delay(uint32_t msec)
{
  osDelay(msec);
}

//--------------------------------------------------------------------+
// Semaphore API
//--------------------------------------------------------------------+
typedef osSemaphoreAttr_t osal_semaphore_def_t;
typedef osSemaphoreId_t osal_semaphore_t;

static inline osal_semaphore_t osal_semaphore_create(osal_semaphore_def_t* semdef)
{
  return osSemaphoreNew(1, 0, semdef);
}

static inline bool osal_semaphore_post(osal_semaphore_t sem_hdl, bool in_isr)
{
  return osSemaphoreRelease(sem_hdl) == osOK;
}

static inline bool osal_semaphore_wait (osal_semaphore_t sem_hdl, uint32_t msec)
{
  uint32_t const ticks = (msec == OSAL_TIMEOUT_WAIT_FOREVER) ? osWaitForever: msec;
  return osSemaphoreAcquire(sem_hdl, ticks) == osOK;
}

static inline void osal_semaphore_reset(osal_semaphore_t const sem_hdl)
{
  osSemaphoreDelete(sem_hdl);
}

//--------------------------------------------------------------------+
// MUTEX API (priority inheritance)
//--------------------------------------------------------------------+
typedef osMutexAttr_t osal_mutex_def_t;
typedef osMutexId_t osal_mutex_t;

static inline osal_mutex_t osal_mutex_create(osal_mutex_def_t* mdef)
{
  return osMutexNew(mdef);
}

static inline bool osal_mutex_lock (osal_mutex_t mutex_hdl, uint32_t msec)
{
  uint32_t const ticks = (msec == OSAL_TIMEOUT_WAIT_FOREVER) ? osWaitForever: msec;
  return osMutexAcquire(mutex_hdl, ticks) == osOK;
}

static inline bool osal_mutex_unlock(osal_mutex_t mutex_hdl)
{
  return osMutexRelease(mutex_hdl) == osOK;
}

//--------------------------------------------------------------------+
// QUEUE API
//--------------------------------------------------------------------+

// role device/host is used by OS NONE for mutex (disable usb isr) only
#define OSAL_QUEUE_DEF(_role, _name, _depth, _type) \
  osal_queue_def_t _name = { .depth = _depth, .item_sz = sizeof(_type), };

typedef struct
{
  uint16_t depth;
  uint16_t item_sz;

  osMessageQueueId_t sq;
}osal_queue_def_t;

typedef osMessageQueueId_t osal_queue_t;

static inline osal_queue_t osal_queue_create(osal_queue_def_t* qdef)
{
  qdef->sq = osMessageQueueNew(qdef->depth, qdef->item_sz, NULL);
  return qdef->sq;
}

static inline bool osal_queue_receive(osal_queue_t qhdl, void* data, uint32_t msec)
{
  return osMessageQueueGet(qhdl, data, 0U, msec) == osOK;
}

static inline bool osal_queue_send(osal_queue_t qhdl, void const * data, bool in_isr)
{
  return osMessageQueuePut(qhdl, data, 0U, osWaitForever) == osOK;
}

static inline bool osal_queue_empty(osal_queue_t qhdl)
{
  return osMessageQueueGetCount(qhdl) == 0;
}
