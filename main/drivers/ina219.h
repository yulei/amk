/**
 * @file ina219.h
 * 
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#define INA219_ADDR_GND_GND         0x80    // A1_GND, A0_GND
#define INA219_ADDR_VCC_GND         0x88    // A1_VCC, A0_GND
#define INA219_ADDR_GND_VCC         0x82    // A1_GND, A0_GND
#define INA219_ADDR_VCC_VCC         0x8A    // A1_GND, A0_GND

#ifndef INA219_ADDR
#define INA219_ADDR                 INA219_ADDR_GND_GND
#endif

// reset
#define INA219_RESET                0x8000  // reset the chip

// bus voltage range
#define BUS_VOLTAGE_16V             0x0000  // 0-16V Range
#define BUS_VOLTAGE_32V             0x2000  // 0-32V Range

// gain
#define GAIN_1_40MV                 0x0000  // Gain 1, 40mV Range
#define GAIN_2_80MV                 0x0800  // Gain 2, 80mV Range
#define GAIN_4_160MV                0x1000  // Gain 4, 160mV Range
#define GAIN_8_320MV                0x1800  // Gain 8, 320mV Range

// bus and shunt adc
#define ADC_9BIT_1S_84US            0x0000  // 1 x 9-bit sample
#define	ADC_10BIT_1S_148US          0x0001  // 1 x 10-bit sample
#define	ADC_11BIT_1S_276US          0x0002  // 1 x 11-bit sample
#define	ADC_12BIT_1S_532US          0x0003  // 1 x 12-bit sample

#define	ADC_12BIT_2S_1060US         0x0009  // 2 x 12-bit samples averaged together
#define	ADC_12BIT_4S_2130US         0x000A  // 4 x 12-bit samples averaged together
#define	ADC_12BIT_8S_4260US         0x000B  // 8 x 12-bit samples averaged together
#define	ADC_12BIT_16S_8510US        0x000C  // 16 x 12-bit samples averaged together
#define	ADC_12BIT_32S_17MS          0x000D  // 32 x 12-bit samples averaged together
#define	ADC_12BIT_64S_34MS          0x000E  // 64 x 12-bit samples averaged together
#define	ADC_12BIT_128S_69MS         0x000F  // 128 x 12-bit samples averaged together

#define BUS_ADC_OFFSET              0x07    // offset of the bus adc
#define SHUNT_ADC_OFFSET            0x03    // offset of the shunt adc

// operation mode
#define MODE_POWERDOWN              0x0000  // power down
#define MODE_SHUNT_TRIGGERED        0x0001  // shunt trigger
#define MODE_BUS_TRIGGERED          0x0002  // bus trigger
#define MODE_SHUNT_BUS_TRIGGERED    0x0003  // shunt and bus trigger
#define MODE_ADC_OFF                0x0004  // adc off
#define MODE_SHUNT_CONTINUOUS       0x0005  // shunt continuous
#define MODE_BUS_CONTINUOUS         0x0006  // bus continuous
#define MODE_SHUNT_BUS_CONTINUOUS   0x0007  // shunt and bus continuous

#define INA219_CONFIG_DEFAULT  (BUS_VOLTAGE_16V|GAIN_1_40MV|(ADC_12BIT_4S_2130US<<BUS_ADC_OFFSET)|(ADC_12BIT_4S_2130US<<SHUNT_ADC_OFFSET)|MODE_SHUNT_BUS_CONTINUOUS)
#define INA219_CALIBRATION_DEFAULT

bool ina219_init(uint8_t addr, uint16_t config);

bool ina219_set_calibration(uint8_t addr, uint16_t calibration);

bool ina219_get_bus_voltage(uint8_t addr, uint16_t* voltage);
bool ina219_get_shunt_voltage(uint8_t addr, uint16_t* voltage);
bool ina219_get_current(uint8_t addr, uint16_t* current);
bool ina219_get_power(uint8_t addr, uint16_t* power);

void ina219_uninit(uint8_t addr);
