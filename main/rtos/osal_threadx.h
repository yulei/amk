/**
 * @file osal_cmsis_os2.h
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdint.h>

#include "tx_api.h"

extern TX_BYTE_POOL* g_tx_byte_pool;

//--------------------------------------------------------------------+
// TASK API
//--------------------------------------------------------------------+
static inline void osal_task_delay(uint32_t msec)
{
  tx_thread_sleep(msec);
}

//--------------------------------------------------------------------+
// Semaphore API
//--------------------------------------------------------------------+
typedef uint8_t osal_semaphore_def_t;
typedef TX_SEMAPHORE* osal_semaphore_t;

static inline osal_semaphore_t osal_semaphore_create(osal_semaphore_def_t* semdef)
{
  osal_semaphore_t sema = NULL;
  if (TX_SUCCESS == tx_byte_allocate(g_tx_byte_pool, (void**)&sema, sizeof(TX_SEMAPHORE), TX_NO_WAIT)) {
    if (TX_SUCCESS != tx_semaphore_create(sema, NULL, 0)) {
      tx_byte_release(sema);
      sema = NULL;
    }
  }

  return sema;
}

static inline bool osal_semaphore_post(osal_semaphore_t sem_hdl, bool in_isr)
{
  return tx_semaphore_put(sem_hdl) == TX_SUCCESS;
}

static inline bool osal_semaphore_wait (osal_semaphore_t sem_hdl, uint32_t msec)
{
  uint32_t const ticks = (msec == OSAL_TIMEOUT_WAIT_FOREVER) ? TX_WAIT_FOREVER: msec;
  return tx_semaphore_get(sem_hdl, ticks) == TX_SUCCESS;
}

static inline void osal_semaphore_reset(osal_semaphore_t const sem_hdl)
{
  tx_semaphore_delete(sem_hdl);
  tx_byte_release(sem_hdl);
}

//--------------------------------------------------------------------+
// MUTEX API (priority inheritance)
//--------------------------------------------------------------------+
typedef uint8_t osal_mutex_def_t;
typedef TX_MUTEX* osal_mutex_t;

static inline osal_mutex_t osal_mutex_create(osal_mutex_def_t* mdef)
{
  osal_mutex_t mutex = NULL;
  if (TX_SUCCESS == tx_byte_allocate(g_tx_byte_pool, (void**)&mutex, sizeof(TX_MUTEX), TX_NO_WAIT)) {
    if (TX_SUCCESS != tx_mutex_create(mutex, NULL, TX_NO_INHERIT)) {
      tx_byte_release(mutex);
      mutex = NULL;
    }
  }
  return mutex;
}

static inline bool osal_mutex_lock (osal_mutex_t mutex_hdl, uint32_t msec)
{
  uint32_t const ticks = (msec == OSAL_TIMEOUT_WAIT_FOREVER) ? TX_WAIT_FOREVER : msec;

  return tx_mutex_get(mutex_hdl, ticks) == TX_SUCCESS;
}

static inline bool osal_mutex_unlock(osal_mutex_t mutex_hdl)
{
  return tx_mutex_put(mutex_hdl) == TX_SUCCESS;
}

//--------------------------------------------------------------------+
// QUEUE API
//--------------------------------------------------------------------+

typedef TX_QUEUE* osal_queue_t;

// role device/host is used by OS NONE for mutex (disable usb isr) only
#define OSAL_QUEUE_DEF(_role, _name, _depth, _type) \
  osal_queue_def_t _name = { .depth = _depth, .item_sz = sizeof(_type), };

typedef struct
{
  uint16_t depth;
  uint16_t item_sz;
  osal_queue_t sq;
}osal_queue_def_t;


static inline osal_queue_t osal_queue_create(osal_queue_def_t* qdef)
{
  osal_queue_t queue = NULL;
  if (TX_SUCCESS == tx_byte_allocate(g_tx_byte_pool, (void**)&queue, sizeof(TX_QUEUE), TX_NO_WAIT)) {
    uint32_t size = (qdef->item_sz+3) / 4;
    void* queue_start = NULL;
    uint32_t queue_size = qdef->depth*size*sizeof(uint32_t);
    if (TX_SUCCESS == tx_byte_allocate(g_tx_byte_pool, &queue_start, queue_size, TX_NO_WAIT)) {
      if (TX_SUCCESS == tx_queue_create(queue, NULL, size, queue_start, queue_size)) {
        qdef->sq = queue;
      } else {
        tx_byte_release(queue_start);
        tx_byte_release(queue);
      }
    } else {
      tx_byte_release(queue);
    }
  }
  return qdef->sq;
}

static inline bool osal_queue_receive(osal_queue_t qhdl, void* data, uint32_t msec)
{
  uint32_t const ticks = (msec == OSAL_TIMEOUT_WAIT_FOREVER) ? TX_WAIT_FOREVER : msec;
  return tx_queue_receive(qhdl, data, ticks) == TX_SUCCESS;
}

static inline bool osal_queue_send(osal_queue_t qhdl, void const * data, bool in_isr)
{
  return tx_queue_send(qhdl, (void*)data, TX_NO_WAIT) == TX_SUCCESS;
}

static inline bool osal_queue_empty(osal_queue_t qhdl)
{
  uint32_t count = 0;
  tx_queue_info_get(qhdl, NULL, &count, NULL, NULL, NULL, NULL);
  return count == 0;
}
