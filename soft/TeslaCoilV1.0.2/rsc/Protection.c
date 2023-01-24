#include "Protection.h"

uint16_t valuesAdc[2], minVoltage = 0, minTemp = 0, maxTemp = 0;
bool isTermalProtect = false, isLowVoltageProtect = false, isOffOutPin = false, TriggerOCD = false, TripOCD = false;
uint8_t countTransVal = 0;

FilterData voltageFilter, temperatureFilter;

void setEnableOutPin() {
  if(isOffOutPin == false && TriggerOCD == false) OUT_PORT->BSRR |= (1 << OUT_PIN);
}

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
  
  voltageFilter.A = 7;
  voltageFilter.K = 3;
  
  temperatureFilter.A = 7;
  temperatureFilter.K = 3;
  
  minVoltage = convertVoltageToAdc(MIN_POWER_VOLTAGE, VAL_R1_DIVIDER, VAL_R2_DIVIDER);
  minTemp = convertTemperatureToAdc(MIN_TEMPERATURE);
  maxTemp = convertTemperatureToAdc(MAX_TEMPERATURE);
  
  GPIOA->MODER &= ~GPIO_MODER_MODER2;
  GPIOA->PUPDR |= GPIO_PUPDR_PUPDR2_0;
  
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  
  EXTI->IMR |= EXTI_IMR_MR2;
  EXTI->RTSR |= EXTI_RTSR_TR2;
  EXTI->FTSR |= EXTI_FTSR_TR2;
  SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PA;

  NVIC_SetPriority(EXTI2_3_IRQn, 0);
  NVIC_EnableIRQ(EXTI2_3_IRQn);
}

void EXTI2_3_IRQHandler() {
  if((EXTI->IMR & EXTI_IMR_MR2) && (EXTI->PR & EXTI_PR_PR2)) {
    EXTI->PR |= EXTI_PR_PR2;
    
    if(GPIOA->IDR & GPIO_IDR_2) {
      TriggerOCD = false;
    } else {
      OUT_PORT->BRR |= (1 << OUT_PIN);
      TriggerOCD = true;
      TripOCD = true;
    }
  }
}

void DMA1_Channel1_IRQHandler() {
  if(DMA1->ISR & DMA_ISR_TCIF1) {
    DMA1->IFCR |= DMA_IFCR_CTCIF1;
  }
}

#define getPowerVoltage() valuesAdc[1]
#define getTemperatureValue() valuesAdc[0]

void TpansmitValuesProtection() {
  uint16_t temp = 4096 - getTemperatureValue();
  uint8_t frame[] = {TX_SET_TEMP, temp >> 7, temp & 0x7f, (isTermalProtect == true)? 1 : 0};
  TransmitFrame(frame);
  
  float volt = ((((float) getPowerVoltage()) / 4096.0f) * 3.3f) / VAL_DIVIDER;
  uint16_t valVolt = (uint16_t) (volt * 100.0f);
  
  uint8_t frame2[] = {TX_SET_POWER, valVolt >> 7, valVolt & 0x7f, (isLowVoltageProtect == true)? 1 : 0};
  TransmitFrame(frame2);
}

uint8_t ignoreSamples = 0;

void processProtection() {
  uint16_t volt = filterData(getPowerVoltage(), &voltageFilter);
  uint16_t temp = filterData(getTemperatureValue(), &temperatureFilter);
  
  if(ignoreSamples < 10) {
    ignoreSamples++;
    return;
  }
    
  if(temp < maxTemp) {
    isTermalProtect = true;
  } else {
    if(isTermalProtect == true) {
      if(temp > minTemp) isTermalProtect = false;
    }
  }
    
  if(volt <= minVoltage) {
    isLowVoltageProtect = true;
  } else {
    isLowVoltageProtect = false;
  }
  
  if(isLowVoltageProtect == true || isTermalProtect == true) {
    if(isOffOutPin == false) {
      isOffOutPin = true;
      setStopMidiPlayer();
      offInterrupter();
    }    
  } else isOffOutPin = false;
}