#include "ProcessInterrupterData.h"

bool isEnableModulater = false, isEnableEnterrupter = false;
uint16_t timeOnModulater = 0, timeOffModulater = 0, CountTimeOnModulater = 0, CountTimeOffModulater = 0;
uint8_t dutyModulater, freqMolulater;

void offInterrupter() {
  isEnableEnterrupter = false;
  isEnableModulater = false;
  setEnableTimer(false, 0, false);
}

void setEnbale(uint8_t *buff) {
  isEnableEnterrupter = (buff[1] > 0)? true : false;
  setEnableTimer(isEnableEnterrupter, 0, false);
  isEnableModulater = (dutyModulater < 100 && isEnableEnterrupter == true)? true : false;
}

void setPulse(uint8_t *buff) {
  setOnePulseTimer((buff[1] << 7) | buff[2], 0);
}

void setPW(uint8_t *buff) {
  setPulseTimer((buff[1] << 7) | buff[2], 0);
}

void setFrequency(uint8_t *buff) {
  setFreqdTimer((buff[1] << 7) | buff[2], 0);
}

void setModulater(uint8_t *buff) {
  dutyModulater = buff[1];
  freqMolulater = buff[2];
  
  if(dutyModulater < 100) {
    uint16_t p = 1000 / freqMolulater;
    timeOnModulater = (p * ((dutyModulater < 1)? 1 : dutyModulater)) / 100;
    timeOffModulater = p - timeOnModulater;
    
    isEnableModulater = isEnableEnterrupter;
    
  } else {
    isEnableModulater = (dutyModulater < 100 && isEnableEnterrupter == true)? true : false;
    setEnableTimer(isEnableEnterrupter, 0, false);
  }
}

void processModulater() {
  if(isEnableModulater == true && isEnableEnterrupter == true) {
    if(CountTimeOnModulater > 0) {
      CountTimeOnModulater--;
      setEnableTimer(true, 0, false);
    } else 
    if(CountTimeOffModulater > 0) {
      CountTimeOffModulater--;
      setEnableTimer(false, 0, false);
    } else {
      CountTimeOffModulater = timeOffModulater;
      CountTimeOnModulater = timeOnModulater;
    }
  }
}