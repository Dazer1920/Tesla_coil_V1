#ifndef _PROTECTION_H
#define _PROTECTION_H

#include "System.h"
#include "LED.h"
#include "ProcessInterrupterData.h"
  
#define VALUE_RESISTANCE_R1 3000
#define VALUE_RESISTANCE_R2 1000

#define MIN_VOLTAGE 5.5

void startProtection();

void processProtection();

uint16_t getTempValue();
uint16_t getPowerValue();

#endif // _PROTECTION_H