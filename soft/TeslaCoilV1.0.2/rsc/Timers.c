#include "Timers.h"

extern uint8_t Volume;
extern bool portamentoType;
extern uint16_t portamentoTime;
extern uint16_t AttackVal, DecayVal, SustainVal, ReleaseVal;

Timer Timers[NUMBER_VOICES];

void initTimers() {
  RCC->APB2ENR |= RCC_APB2ENR_TIM16EN | RCC_APB2ENR_TIM17EN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;

  TIM14->ARR = 999UL;
  TIM14->PSC = 47;
  TIM14->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;
  
  TIM16->ARR = 999UL;
  TIM16->PSC = 47;
  TIM16->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;
  
  TIM17->ARR = 999UL;
  TIM17->PSC = 47;
  TIM17->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;
  
  NVIC_SetPriority(TIM14_IRQn, 2);
  NVIC_EnableIRQ(TIM14_IRQn);
  
  NVIC_SetPriority(TIM16_IRQn, 2);
  NVIC_EnableIRQ(TIM16_IRQn);
  
  NVIC_SetPriority(TIM17_IRQn, 2);
  NVIC_EnableIRQ(TIM17_IRQn);
  
  TIM_TypeDef *tims[] = {TIM14, TIM16, TIM17};
  for(uint8_t i = 0; i < NUMBER_VOICES; i++) {
    Timers[i].tim = tims[i];
    Timers[i].enable = false;
    Timers[i].periodArr = 499;
    Timers[i].period = 499;
    Timers[i].pulse = 50;
    Timers[i].pulseDel = 50;
  }
}

void setEnableTimer(bool e, uint8_t n, bool isMidi) {
  Timers[n].isMidi = isMidi;
  Timers[n].tim->ARR = Timers[n].period;
  Timers[n].countTime = 0;
  
  OUT_PORT->ODR &= ~(1 << OUT_PIN);
  
  if(isMidi == false) {
    Timers[n].enable = e;
    Timers[n].tim->DIER = (e == true)? 
      Timers[n].tim->DIER | TIM_DIER_UIE : 
      Timers[n].tim->DIER & ~TIM_DIER_UIE;
  } else {
    if(e == true){
      Timers[n].enable = true;
      Timers[n].statusAdsr = ATTACK;
      
      if(AttackVal > 0) { 
        Timers[n].countPulse = 0.0f;
        Timers[n].countPulseUint = ((float) Timers[n].pulse) / ((float) AttackVal);
      } else {
        if(DecayVal > 0) Timers[n].countPulse = Timers[n].pulse;
        else {
          Timers[n].pulseSustain = (float) (Timers[n].pulse * SustainVal) / 100;
          Timers[n].countPulse = Timers[n].pulseSustain;
          Timers[n].statusAdsr = SUSTAIN;
        }
      }
      
      Timers[n].tim->DIER |= TIM_DIER_UIE;
    } else {
      if(ReleaseVal > 0) {
        Timers[n].statusAdsr = RELEASE;
        Timers[n].countPulseUint = ((float) Timers[n].countPulse) / ((float) ReleaseVal);
      } else Timers[n].enable = false;
    }
  }
}

void ResetAllTimers() {
  for(uint8_t i = 0; i < 3; i++) {
    Timers[i].enable = false;
    Timers[i].isMidi = false;
    Timers[i].tim->DIER &= ~TIM_DIER_UIE;
  }
}

void setOnePulseTimer(uint16_t pulse, uint8_t n) {
  Timers[n].pulse = pulse;
  Timers[n].pulseDel = pulse;
  Timers[n].isMidi = false;
  
  Timers[n].enable = false;
  Timers[n].tim->ARR = 9999;
  Timers[n].tim->DIER |= TIM_DIER_UIE;
}

void setPulseTimer(uint16_t p, uint8_t n) {
  Timers[n].pulse = p;
}

void setPeriodTimer(uint32_t p, uint8_t n) {
  Timers[n].period = p;
  Timers[n].tim->ARR = Timers[n].period;
}

void setFreqdTimer(uint32_t f, uint8_t n) {
  Timers[n].period = (1000000UL / f) - 1;
  Timers[n].tim->ARR = Timers[n].period;
}

void delayUs(uint16_t us) {
  uint32_t ticks_delay = us;
  uint32_t ticks_tim = TIM3->CNT;
  while(ticks_delay) {
    if(TIM3->CNT != ticks_tim){
      ticks_tim = TIM3->CNT;
       ticks_delay--;
    }
  }
}

void processTimer(Timer *timer) {
  if(timer->tim->SR & TIM_SR_UIF) {
    timer->tim->SR &= ~TIM_SR_UIF;
    
    if(timer->isMidi == true) {
      timer->pulseDel = (((uint16_t) timer->countPulse) * Volume) / 100;
      
      if(timer->periodArr != timer->period) {
        if(portamentoTime == 0) {
          timer->periodArr = timer->period;
          timer->countTime = 0;
        } else {
          if(timer->countTime == 0) {
            if(timer->periodArr == 0) timer->periodArr = timer->period;
            
            uint32_t delta = abs(((int32_t) timer->period) - ((int32_t) timer->periodArr));
            timer->countTime = delta / portamentoTime;
            timer->countTime = (timer->countTime == 0)? 1 : timer->countTime;
          }
        }
      }
      
    } else {
      timer->periodArr = timer->period;
      timer->pulseDel = timer->pulse;
    }
    
    setEnableOutPin();
    delayUs(timer->pulseDel);
    OUT_PORT->BRR |= (1 << OUT_PIN);
    
    timer->tim->ARR = timer->periodArr;
    if(timer->enable == false) {
      timer->tim->DIER &= ~TIM_DIER_UIE;
      OUT_PORT->BRR |= (1 << OUT_PIN);
    }
  }
}

void TIM14_IRQHandler() {
  processTimer(&Timers[0]);
}

void TIM16_IRQHandler() {
  processTimer(&Timers[1]);
}

void TIM17_IRQHandler() {
  processTimer(&Timers[2]);
}