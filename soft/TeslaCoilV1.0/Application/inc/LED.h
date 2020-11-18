#ifndef _LED_H
#define _LED_H

#include "System.h"

typedef enum {
  NORMAL_LED = 0,
  BT_CHECK_BAUD_LED,
  BT_CONFIGURED_LED,
  BT_CONFIG_LED,
  REBOOT_SYSTEM_LED,
  RESET_SYSTEM_LED,
  ERROR_HANDLER_LED,
  ERROR_SYSTEM_LED,
  TERMO_PROTECT_LED,
  LOW_POWER_LED,

} StatusLed;

void setStatusLed(StatusLed status);
void processLed();

#endif // _LED_H