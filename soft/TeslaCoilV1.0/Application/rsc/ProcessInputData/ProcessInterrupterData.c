#include "ProcessInterrupterData.h"

Enable isEnableModulater = OFF, isEnableInterrupter = OFF;
uint16_t modOnTime, modOffTime, modCountOnTime, modCounOffTime;
uint16_t period, pulseWhidth;

void setEnableInterrupter(uint8_t *data) {
  isEnableInterrupter = (data[2] > 0)? ON : OFF;
  
  HAL_TIM_Base_Start(&htim14);
  if(isEnableInterrupter == ON) HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);
  else HAL_TIM_PWM_Stop(&htim14, TIM_CHANNEL_1);
}

void setPwInterrupter(uint8_t *data) {
  pulseWhidth = (data[2] << 7) | data[3];
  
  pulseWhidth = (pulseWhidth > MAX_PULSE)? MAX_PULSE : pulseWhidth;
  pulseWhidth = (pulseWhidth < 2)? 2 : pulseWhidth;
  
  htim14.Instance->CCR1 = pulseWhidth - 1;
}

void setFreqInterrupter(uint8_t *data) {
  period = (data[2] << 7) | data[3];
  
  period = (period > MAX_FREQ)? MAX_FREQ : period;
  period = (period < 20)? 20 : period;
  
  period = 1000000 / period;
  htim14.Instance->ARR = period - 1;
}

void setModInterrupter(uint8_t *data) {
  if(data[2] < 100) {
    uint16_t p = 1000 / data[3];
    modOnTime = (p * ((data[2] < 1)? 1 : data[2])) / 100;
    modOffTime = p - modOnTime;
    
    isEnableModulater = ON;
  } else {
    isEnableModulater = OFF;
    if(isEnableInterrupter == ON) HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);
  }
}

void setPulseInterrupter(uint8_t *data) {
  
}

void readInterrupterData(uint8_t *data) {
  Enable fault = OFF;
  
  switch(data[1]) {
  default: fault = ON; break;
  
  case RX_SET_PULSE_WHITH: setPwInterrupter(data); break;
  case RX_SET_FREQUENCY: setFreqInterrupter(data); break;
  case RX_SET_MODULATER: setModInterrupter(data); break;
  case RX_SET_PULSE: setPulseInterrupter(data); break;
  case RX_SET_ENABLE: setEnableInterrupter(data); break;
  }
  
  if(fault == OFF) cleareConterConnectTimer();
}

void processModulaterInterrupter() {
  if(isEnableModulater == OFF || isEnableInterrupter == OFF) return;
  
  if(modCounOffTime > 0) {
    modCounOffTime--;
    HAL_TIM_PWM_Stop(&htim14, TIM_CHANNEL_1);
  } else 
  if(modCountOnTime > 0) {
    modCountOnTime--;
    HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);
  } else {
    modCounOffTime = modOffTime;
    modCountOnTime = modOnTime;
  }
}

void OffInterrupter() {
  isEnableInterrupter = OFF;
  HAL_TIM_PWM_Stop(&htim14, TIM_CHANNEL_1);
}