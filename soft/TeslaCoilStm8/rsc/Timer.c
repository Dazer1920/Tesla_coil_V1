#include "Timer.h"

void initTimer() {
  TIM2->PSCR = 0x04;
  TIM2->CR1 |= TIM2_CR1_CEN;
}

void EnablePulse(uint8_t e) {
  if(e) TIM2->IER |= TIM2_IER_UIE;
  else TIM2->IER &= ~TIM2_IER_UIE;
}

uint16_t Pulse;

void SetPulseWhith(uint16_t p) {
  Pulse = p;
}

void setFreq(uint16_t f) {
  uint16_t p = 1000000 / f;

  TIM2->ARRH = p >> 8;
  TIM2->ARRL = p & 0x00ff;
}

void delayUs(uint16_t us) {
  for (uint32_t i = 0; i < ((F_CPU / 18000000UL) * us); i++) {
    asm("nop");
  }
}

#pragma vector = 15
__interrupt void Pin6_interrupt(void) {
 if(TIM2->SR1 & TIM2_SR1_UIF) {
    TIM2->SR1 &= ~TIM2_SR1_UIF;

    OUT_PORT->ODR |= (1 << OUT_PIN);
    delayUs(Pulse);
    OUT_PORT->ODR &= ~(1 << OUT_PIN);
  }
}