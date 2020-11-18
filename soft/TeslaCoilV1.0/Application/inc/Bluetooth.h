#ifndef _BLUETOOTH_H
#define _BLUETOOTH_H

#include "System.h"
#include "Serial.h"
#include "LED.h"

#define CHECK_AT_COMMAND "AT"
#define CHECK_ANSWER_BT "OK"

#define ANSWER_CONFIG "OK"


#define START_TIMEOUT 2000 // ms
#define TIMEOUT_BETWEN_REQUESTS 1000 // ms

#define NUMBER_REQUEST_IN_CHECK_BAUD 2
#define NUMBER_REQUEST_IN_CONFIG 2

typedef enum {
  NO_EVENT = 0,
  CHECK_BAUD,
  BT_CONFIGURED,
  BT_ANSWER_CHECK,
  BT_ANSWER_CONFIG,
  CONFIGURATE_BT,
  
  DISCONNECTED,
  CHANGE_CONNECTION,
  CONNECTED,
  
} BluetoothConfigEvent;

void setBtEvent(BluetoothConfigEvent event);
void setEventTimer(uint32_t ms, BluetoothConfigEvent event);
void processBluetoothEventTimer();

void startConfigBluetooth();
void processBluetooth();

#endif // _BLUETOOTH_H