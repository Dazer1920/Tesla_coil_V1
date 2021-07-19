#ifndef _LED_H
#define _LED_H

#include "System.h"

typedef enum {
  LED_OFF = 0,
  LED_ON,
  LED_BLINK,

} LedState;

void setStateLed(LedState s);

void processLed();

#endif // _LED_H