#include "System.h"

boolean isError = false;

void initClock() {
  if((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {
    RCC->CFGR &= ~RCC_CFGR_SW;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
  }
  
  RCC->CR |= RCC_CR_HSION;
  while(!(RCC->CR & RCC_CR_HSIRDY));
  
  RCC->CFGR |= RCC_CFGR_PLLMUL12;
  RCC->CFGR &= ~RCC_CFGR_PLLSRC;
  
  RCC->CR |= RCC_CR_PLLON;
  while(!(RCC->CR & RCC_CR_PLLRDY));
  
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void initSystemTimer() {
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  
  TIM3->ARR = 999UL;
  TIM3->PSC = 47;
  TIM3->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;
}

void initGPIO() {
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  LED_PORT->MODER |= (1 << (2 * LED_PIN));
  OUT_PORT->MODER |= (1 << (2 * OUT_PIN));
  OUT_PORT->ODR &= ~(1 << OUT_PIN);
}

void SystemError() {
  LED_PORT->ODR &= ~(1 << LED_PIN);
  OUT_PORT->ODR &= ~(1 << OUT_PIN);
  
  TIM3->CR1 &= ~TIM_CR1_CEN;
  
  __disable_irq();
}