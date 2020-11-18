#ifndef _LED_H
#define _LED_H

#include "System.h"
#include "Protection.h"

#define ON_LED LED_PORT->ODR |= (1 << LED_PIN)
#define OFF_LED LED_PORT->ODR &= ~(1 << LED_PIN)
#define INVERT_LED LED_PORT->ODR ^= (1 << LED_PIN)

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