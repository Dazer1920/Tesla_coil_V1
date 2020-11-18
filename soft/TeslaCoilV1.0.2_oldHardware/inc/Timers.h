#ifndef _TIMERS_H
#define _TIMERS_H

#include "System.h"
#include "Util.h"
#include "ProcessMidiEvents.h"

typedef enum {
  DISABLE_ADSR = 0,
  ATTACK,
  DECAY,
  SUSTAIN,
  RELEASE,
  ADSR_END
} StatusAdsr;

typedef struct {
  uint32_t period;
  uint32_t periodArr;
  uint16_t countTime;
  uint16_t pulse;
  float countPulse;
  float countPulseUint;
  float pulseSustain;
  uint16_t pulseDel;
  TIM_TypeDef *tim;
  boolean enable;
  boolean isMidi;
  StatusAdsr statusAdsr;
  
} Timer;

void initTimers();

void ResetAllTimers();
void setEnableTimer(boolean e, uint8_t n, boolean isMidi);
void setOnePulseTimer(uint16_t pulse, uint8_t n);

void setPulseTimer(uint16_t p, uint8_t n);
void setPeriodTimer(uint32_t p, uint8_t n);
void setFreqdTimer(uint32_t f, uint8_t n);

#endif // _TIMERS_H