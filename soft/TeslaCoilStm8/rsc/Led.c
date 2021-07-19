#include "Led.h"

uint8_t countBlinks = 0;
uint16_t countDelay = 0;
LedState currentState = LED_OFF;

void setStateLed(LedState s) {
  currentState = s;
  countBlinks = 0;
  countDelay = 0;
}

void processLed() {
  switch(currentState) {
  default: break;

  case LED_OFF:
    LED_PORT->ODR |= (1 << LED_PIN);
    countBlinks = 0;
    countDelay = 0;
    break;

  case LED_ON:
    LED_PORT->ODR &= ~(1 << LED_PIN);
    countBlinks = 0;
    countDelay = 0;
    break;

  case LED_BLINK:
    if(++countDelay >= 100) {
      LED_PORT->ODR ^= (1 << LED_PIN);
      countBlinks = 0;
      countDelay = 0;
    }
    break;
  }
}