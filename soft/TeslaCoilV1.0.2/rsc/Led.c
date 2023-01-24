#include "Led.h"

StateLed currentStateLed = LED_NO_STATE;
uint16_t countDelLed = 0;
uint8_t countBlink = 0;

extern bool isLowVoltageProtect, isTermalProtect, TriggerOCD, TripOCD;

void setStateLed(StateLed stateLed) {
  currentStateLed = stateLed;
}

void processLed() {
  if(currentStateLed == LED_CONFIG_END) {
    if(countBlink < 6) {
      if(countDelLed >= 100) {
        countDelLed = 0;
        INVERT_GREEN_LED;
        countBlink++;
      } else countDelLed++;
    } else { 
      countBlink = 0;
      countDelLed = 0;
      currentStateLed = LED_NO_STATE;
    }
  } else 
  if(currentStateLed == LED_CONFIGURATE) {
    if(countDelLed >= 100) {
      countDelLed = 0;
      INVERT_GREEN_LED;
    } else countDelLed++;
  } else
  if(TripOCD) {
    if(TriggerOCD) {
      countDelLed = 0;
      OFF_ALL_LED;
      return;
    }
    
    if(countDelLed >= 500) {
      TripOCD = false;
    } else countDelLed++;
  } else 
  if(isLowVoltageProtect == true) {
    ON_RED_LED;
  } else 
  if(isTermalProtect == true) {
    if(countDelLed >= 100) {
      countDelLed = 0;
      INVERT_RED_LED;
    } else countDelLed++;
  } else 
  if(currentStateLed == LED_CHECK_CONFIG) {
    if(countDelLed >= 100) {
      countDelLed = 0;
      currentStateLed = LED_NO_STATE;
    } else {
      countDelLed++;
      OFF_ALL_LED;
    }
  } else 
  if(currentStateLed == LED_DISCONNECT) {
    if(countDelLed >= 100) {
      countDelLed = 0;
      INVERT_RED_LED;
      if(++countBlink >= 6) {
        countBlink = 0;
        currentStateLed = LED_NO_STATE;
      }
    } else countDelLed++;
  } else {
    ON_GREEN_LED;
    countBlink = 0;
    countDelLed = 0;
  }
}