#include "LED.h"

StatusLed currentStatusLed = NORMAL_LED;
uint16_t countDelLed, countBlinkLed;

void setStatusLed(StatusLed status) {
  currentStatusLed = status;
  countBlinkLed = 0;
  countDelLed = 0;
}

void btCheckBaudLed() {
  HAL_GPIO_WritePin(LED_P_GPIO_Port, LED_P_Pin, GPIO_PIN_RESET);
  
  if(++countDelLed >= 100) {
    currentStatusLed = NORMAL_LED;
  } else HAL_GPIO_WritePin(LED_M_GPIO_Port, LED_M_Pin, GPIO_PIN_RESET);
}

void configuredBtLed() {
  HAL_GPIO_WritePin(LED_P_GPIO_Port, LED_P_Pin, GPIO_PIN_RESET);
  
  if(countBlinkLed < 4) {
    if(++countDelLed >= 100) {
      countDelLed = 0;
      countBlinkLed++;
      HAL_GPIO_TogglePin(LED_M_GPIO_Port, LED_M_Pin);
    }
  } else currentStatusLed = NORMAL_LED;
}

void termoProtectLed() {
  HAL_GPIO_WritePin(LED_M_GPIO_Port, LED_M_Pin, GPIO_PIN_RESET);
  
  if(++countDelLed >= 100) {
    countDelLed = 0;
    HAL_GPIO_TogglePin(LED_P_GPIO_Port, LED_P_Pin);
  }
}

void RebootSystemLed() {
  HAL_GPIO_WritePin(LED_P_GPIO_Port, LED_P_Pin, GPIO_PIN_RESET);
  
  if(countBlinkLed < 6) {
    if(++countDelLed >= 80) {
      countDelLed = 0;
      countBlinkLed++;
      HAL_GPIO_TogglePin(LED_M_GPIO_Port, LED_M_Pin);
    }
  } else {
    if(++countDelLed >= 1500) {
      countDelLed = 0;
      countBlinkLed = 0;
    } HAL_GPIO_WritePin(LED_M_GPIO_Port, LED_M_Pin, GPIO_PIN_RESET);
  }
}

void errorHandlerLed() {
  HAL_GPIO_WritePin(LED_M_GPIO_Port, LED_M_Pin, GPIO_PIN_RESET);
  
  if(countBlinkLed < 6) {
    if(++countDelLed >= 80) {
      countDelLed = 0;
      countBlinkLed++;
      HAL_GPIO_TogglePin(LED_P_GPIO_Port, LED_P_Pin);
    }
  } else currentStatusLed = NORMAL_LED;
}

void configBtLed() {
  HAL_GPIO_WritePin(LED_P_GPIO_Port, LED_P_Pin, GPIO_PIN_RESET);
  
  if(++countDelLed >= 50) {
    countDelLed = 0;
    HAL_GPIO_TogglePin(LED_M_GPIO_Port, LED_M_Pin);
  }
}

void processLed() {
  switch(currentStatusLed) {
  default: break;
  
  case NORMAL_LED:
    HAL_GPIO_WritePin(LED_P_GPIO_Port, LED_P_Pin, GPIO_PIN_RESET); 
    HAL_GPIO_WritePin(LED_M_GPIO_Port, LED_M_Pin, GPIO_PIN_SET);
    break;
    
  case LOW_POWER_LED:
    HAL_GPIO_WritePin(LED_P_GPIO_Port, LED_P_Pin, GPIO_PIN_SET); 
    HAL_GPIO_WritePin(LED_M_GPIO_Port, LED_M_Pin, GPIO_PIN_RESET);
    break;
    
  case BT_CONFIGURED_LED: configuredBtLed(); break;
  case BT_CHECK_BAUD_LED: btCheckBaudLed(); break;
  case TERMO_PROTECT_LED: termoProtectLed(); break;
  case REBOOT_SYSTEM_LED: RebootSystemLed(); break;
  case ERROR_HANDLER_LED: errorHandlerLed(); break;
  case BT_CONFIG_LED: configBtLed(); break;
  }
}