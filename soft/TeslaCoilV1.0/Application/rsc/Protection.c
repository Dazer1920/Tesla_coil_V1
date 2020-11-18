#include "Protection.h"

uint32_t buffAdcValues[4];
Enable isTermoProtect = OFF, isLowPower = OFF;
uint16_t minVoltageValue;

void startProtection() {
  HAL_ADC_Start_DMA(&hadc, buffAdcValues, 4);
  
  float divitedVal = ((float)VALUE_RESISTANCE_R2) / ((float)(VALUE_RESISTANCE_R1 + VALUE_RESISTANCE_R2));
  minVoltageValue = (uint16_t) (((((float) MIN_VOLTAGE) * divitedVal) / 3.3f) * 4096.0f);
}

void processProtection() {
  if(getPowerValue() <= minVoltageValue) {
    if(isLowPower == OFF) {
      isLowPower = ON;
      
      OffInterrupter();
      
      setStatusLed(LOW_POWER_LED);
    }
  } else {
    if(isLowPower == ON) {
      isLowPower = OFF;
      setStatusLed(NORMAL_LED);
    }
  }
  
  
}

uint16_t getTempValue() {
  return (uint16_t) buffAdcValues[0];
}

uint16_t getPowerValue() {
  return (uint16_t) buffAdcValues[1];
}