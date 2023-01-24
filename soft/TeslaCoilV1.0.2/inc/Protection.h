#ifndef _PROTECTION_H
#define _PROTECTION_H

#include "System.h"
#include "ProcessSystemData.h"
#include "ProcessInterrupterData.h"
#include "ProcessMidiData.h"

#define TEMP_SENSOR_PORT GPIOA
#define TEMP_SENSOR_PIN 0
#define TEMP_SENSOR_ADC_CHANNEL ADC_CHSELR_CHSEL0

#define POWER_SENSOR_PORT GPIOA
#define POWER_SENSOR_PIN 4
#define POWER_SENSOR_ADC_CHANNEL ADC_CHSELR_CHSEL4

#define VAL_R1_DIVIDER 2000.0f
#define VAL_R2_DIVIDER 1000.0f
#define VAL_DIVIDER (VAL_R2_DIVIDER / (VAL_R2_DIVIDER + VAL_R1_DIVIDER))

#define MIN_POWER_VOLTAGE 7.0f
#define MIN_TEMPERATURE 45.5f
#define MAX_TEMPERATURE 75.5f

void setEnableOutPin();

void initProtection();
void TpansmitValuesProtection();
void processProtection();

#endif // _PROTECTION_H