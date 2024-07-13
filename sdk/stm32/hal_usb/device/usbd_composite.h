/**
 * @file usbd_composite.h
 *  composite device handler
 */

#pragma once

#include <stdbool.h>
#include "usbd_def.h"

extern USBD_ClassTypeDef USBD_COMP;

bool usbd_comp_itf_ready(USBD_HandleTypeDef *pdev, uint32_t itf);
uint8_t usbd_comp_send(USBD_HandleTypeDef *pdev, uint8_t type, uint8_t *report, uint16_t len);

#ifdef MSC_ENABLE

typedef struct _USBD_STORAGE
{
  int8_t (* Init)(uint8_t lun);
  int8_t (* GetCapacity)(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
  int8_t (* IsReady)(uint8_t lun);
  int8_t (* IsWriteProtected)(uint8_t lun);
  int8_t (* Read)(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
  int8_t (* Write)(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
  int8_t (* GetMaxLun)(void);
  int8_t *pInquiry;

} USBD_StorageTypeDef;

#endif