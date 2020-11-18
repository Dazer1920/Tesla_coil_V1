#include "Protection.h"

uint16_t valuesAdc[2], minVoltage = 0, minTemp = 0, maxTemp = 0;
boolean isTermalProtect = false, isLowVoltageProtect = false;
uint8_t countTransVal = 0;

void initProtection() {
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
  
  TEMP_SENSOR_PORT->MODER |= (3 << (2 * TEMP_SENSOR_PIN));
  POWER_SENSOR_PORT->MODER |= (3 << (2 * POWER_SENSOR_PIN));
  
  if ((ADC1->CR & ADC_CR_ADEN) != 0) ADC1->CR |= ADC_CR_ADDIS;
  while ((ADC1->CR & ADC_CR_ADEN) != 0);
  
  ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN;
  ADC1->CR |= ADC_CR_ADCAL;
  while ((ADC1->CR & ADC_CR_ADCAL) != 0);
    
  ADC1->SMPR |= ADC_SMPR_SMP_2;
  ADC1->CHSELR = TEMP_SENSOR_ADC_CHANNEL | POWER_SENSOR_ADC_CHANNEL;
  ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG;
  ADC1->CFGR1 |= ADC_CFGR1_CONT;
  
  DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;
  DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;
  DMA1_Channel1->CCR &= ~DMA_CCR_PL;
  DMA1_Channel1->CCR |= DMA_CCR_CIRC;
  
  DMA1_Channel1->CCR |= DMA_CCR_MINC;
  DMA1_Channel1->CCR &= ~DMA_CCR_PINC;
  DMA1_Channel1->CCR &= ~DMA_CCR_DIR;
  DMA1_Channel1->CCR |= DMA_CCR_TCIE;
  
  DMA1_Channel1->CNDTR = 2;
  DMA1_Channel1->CPAR = (uint32_t) (&ADC1->DR); 
  DMA1_Channel1->CMAR = (uint32_t) (&valuesAdc[0]);
  
  NVIC_SetPriority(DMA1_Channel1_IRQn, 3);
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  
  DMA1_Channel1->CCR |= DMA_CCR_EN;
  ADC1->CR |= ADC_CR_ADSTART | ADC_CR_ADEN;
  
  minVoltage = convertVoltageToAdc(MIN_POWER_VOLTAGE, VAL_R1_DIVIDER, VAL_R2_DIVIDER);
  minTemp = convertTemperatureToAdc(MIN_TEMPERATURE);
  maxTemp = convertTemperatureToAdc(MAX_TEMPERATURE);
}

void DMA1_Channel1_IRQHandler() {
  if(DMA1->ISR & DMA_ISR_TCIF1) {
    DMA1->IFCR |= DMA_IFCR_CTCIF1;
  }
}

uint16_t getPowerVoltage() {
  return valuesAdc[0];
}

uint16_t getTemperatureValue() {
  return valuesAdc[1];
}

void TpansmitValuesProtection() {
  uint16_t temp = 4096 - getTemperatureValue();
  uint8_t frame[] = {TX_SET_TEMP, temp >> 7, temp & 0x7f, (isTermalProtect == true)? 1 : 0};
  TransmitFrame(frame);
  
  float volt = ((((float) getPowerVoltage()) / 4096.0f) * 3.3f) / VAL_DIVIDER;
  uint16_t valVolt = (uint16_t) (volt * 100.0f);
  
  uint8_t frame2[] = {TX_SET_POWER, valVolt >> 7, valVolt & 0x7f, (isLowVoltageProtect == true)? 1 : 0};
  TransmitFrame(frame2);
}

#define NUMBER_VALUES_POWER 10
uint16_t valuesVoltage[NUMBER_VALUES_POWER], valuesTemp[NUMBER_VALUES_POWER];
uint8_t countVoltageValues = 0;

void processProtection() {
  valuesVoltage[countVoltageValues] = getPowerVoltage();
  valuesTemp[countVoltageValues] = getTemperatureValue();
  countVoltageValues++;
  
  if(countVoltageValues >= NUMBER_VALUES_POWER) {
    countVoltageValues = 0;
    
    uint32_t volt = 0, temp = 0;
    for(uint8_t i = 0; i < NUMBER_VALUES_POWER; i++) {
      volt += valuesVoltage[i];
      temp += valuesTemp[i];
    }
    volt /= NUMBER_VALUES_POWER;
    temp /= NUMBER_VALUES_POWER;
    
    if(temp < maxTemp) {
      if(isTermalProtect == false) {
        isTermalProtect = true;
        setStopMidiPlayer();
        offInterrupter(); 
      }
    } else {
      if(isTermalProtect == true) {
        if(temp > minTemp) isTermalProtect = false;
      }
    }
    
    if(volt <= minVoltage) {
      if(isLowVoltageProtect == false) {
        isLowVoltageProtect = true;
        setStopMidiPlayer();
        offInterrupter();
      }
    } else {
      isLowVoltageProtect = false;
    }
  }
}