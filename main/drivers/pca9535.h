/** 
 * @file pca9535.h
 *  interface for pca9535 
 */

#pragma once

#include <stdint.h>

#ifndef PCA9535_ADDR
    #define PCA9535_ADDR 0x40
#endif

#define PCA9535_PIN_DEF(port, pin) ((((port)&0x0F)<<4) | ((pin)&0x0f))
#define PCA9535_PIN(pp) ((pp)&0x0F)
#define PCA9535_PORT(pp) (((pp)>>4)&0x0F)

typedef enum {
    PCA9535_PORT0 = 0,
    PCA9535_PORT1,
} PCA9535_PORT;

void pca9535_init(void);

void pca9535_write_config(PCA9535_PORT port, uint8_t data);
void pca9535_write_polarity(PCA9535_PORT port, uint8_t data);

void pca9535_write_port(PCA9535_PORT port, uint8_t data);
uint8_t pca9535_read_port(PCA9535_PORT port);
