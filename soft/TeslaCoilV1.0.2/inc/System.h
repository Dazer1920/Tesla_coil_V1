#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <stm32f0xx.h>

typedef enum {
  false = 0,
  true
} bool;

#define LED_P_PIN 6
#define LED_P_PORT GPIOA

#define LED_M_PIN 7
#define LED_M_PORT GPIOA

#define OUT_PIN 1
#define OUT_PORT GPIOA

#define BT_RES_PORT GPIOB
#define BT_RES_PIN 1

void initClock();
void initSystemTimer();
void initGPIO();

void SystemError();

void delayMs(uint32_t milis);

#endif // _SYSTEM_H