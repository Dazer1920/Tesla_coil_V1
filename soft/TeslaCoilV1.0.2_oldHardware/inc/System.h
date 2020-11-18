#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <stm32f0xx.h>

typedef enum {
  false = 0,
  true
} boolean;

#define LED_PIN 4
#define LED_PORT GPIOA

#define OUT_PIN 1
#define OUT_PORT GPIOA

void initClock();
void initSystemTimer();
void initGPIO();

void SystemError();

#endif // _SYSTEM_H