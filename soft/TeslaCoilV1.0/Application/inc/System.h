#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "adc.h"
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "Bluetooth.h"
#include "Serial.h"
#include "LED.h"
#include "ProcessSystemData.h"
#include "Connection.h"

typedef enum {
  OFF = 0,
  ON
} Enable;

typedef enum {
  NO_INIT_SYSTEM = 0,
  CONFIG_BLUETOOTH,
  PROCESS_APP,
  SYSTEM_ERROR,
  SYSTEM_RESET,
  REBOOT_SYSTEM
} SystemState;

void setReboot();
void noBluetooth();
void startApp();

void processSystem();

#endif // _SYSTEM_H