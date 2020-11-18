#include "System.h"

SystemState systemState = NO_INIT_SYSTEM;

void noInitSystem() {
  HAL_TIM_Base_Start(&htim16);
  initSerial();
  startProtection();
  systemState = CONFIG_BLUETOOTH;
  startConfigBluetooth();
}

void startApp() {
  systemState = PROCESS_APP;
  setStatusLed(BT_CONFIGURED_LED);
}

void setReboot() {
  systemState = PROCESS_APP;
  setStatusLed(REBOOT_SYSTEM_LED);
}

void noBluetooth() {
  systemState = SYSTEM_ERROR;
}

void processApp() {
  processConnection();
  processProtection();
}

void systemError() {
  HAL_GPIO_WritePin(LED_M_GPIO_Port, LED_M_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_P_GPIO_Port, LED_P_Pin, GPIO_PIN_RESET);
  while(1);
}

void systemReset() {
  NVIC_SystemReset();
}

void rebootSystem() {
  
}

void processSystem() {
  switch(systemState) {
  default: break;
  case NO_INIT_SYSTEM: noInitSystem(); break;
  case CONFIG_BLUETOOTH: processBluetooth(); break;
  case PROCESS_APP: processApp(); break;
  case SYSTEM_ERROR: systemError(); break;
  case SYSTEM_RESET: systemReset(); break;
  case REBOOT_SYSTEM: rebootSystem(); break;
  }
}