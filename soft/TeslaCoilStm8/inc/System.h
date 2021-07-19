#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "stm8s.h"

#define F_CPU 16000000UL

#define LED_PORT GPIOA
#define LED_PIN 3

#define OUT_PORT GPIOC
#define OUT_PIN 4

#define DISCONNECT_PORT GPIOC
#define DISCONNECT_PIN 3

void initClock();
void initGpio();
void initSystemTimer();

#endif // _SYSTEM_H