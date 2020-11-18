#ifndef _LED_H
#define _LED_H

#include "System.h"
#include "Protection.h"

#define ON_GREEN_LED LED_M_PORT->ODR |= (1 << LED_M_PIN); LED_P_PORT->ODR &= ~(1 << LED_P_PIN)
#define ON_RED_LED LED_P_PORT->ODR |= (1 << LED_P_PIN); LED_M_PORT->ODR &= ~(1 << LED_M_PIN)
#define OFF_ALL_LED LED_P_PORT->ODR &= ~(1 << LED_P_PIN); LED_P_PORT->ODR |= (1 << LED_P_PIN)
#define INVERT_RED_LED LED_P_PORT->ODR ^= (1 << LED_P_PIN); LED_M_PORT->ODR &= ~(1 << LED_M_PIN)
#define INVERT_GREEN_LED LED_M_PORT->ODR ^= (1 << LED_M_PIN); LED_P_PORT->ODR &= ~(1 << LED_P_PIN)

typedef enum {
  LED_NO_STATE = 0,
  LED_CHECK_CONFIG,
  LED_CONFIG_END,
  LED_CONFIGURATE,
  LED_DISCONNECT,

} StateLed;

void setStateLed(StateLed stateLed);
void processLed();

#endif // _LED_H