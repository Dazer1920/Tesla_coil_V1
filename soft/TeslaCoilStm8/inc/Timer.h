#ifndef _TIMER_H
#define _TIMER_H

#include "System.h"

void initTimer();

void EnablePulse(uint8_t e);
void SetPulseWhith(uint16_t p);
void setFreq(uint16_t f);

#endif // _TIMER_H