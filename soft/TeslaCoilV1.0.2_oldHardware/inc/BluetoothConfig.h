#ifndef _BLUETOOTH_CONFIG
#define _BLUETOOTH_CONFIG

#include "System.h"
#include "string.h"
#include "Serial.h"
#include "Led.h"
#include "ProcessInterrupterData.h"
#include "ProcessMidiData.h"

#define START_TIMEOUT 2000
#define RESPONSE_TIME 1000
#define RESPONSE_TIME_CONFIGURATE 1200
#define NUMBER_REQUESTS 2

#define CHECK_CONFIG_COMMAND_REQUEST "AT"
#define CHECK_CONFIG_COMMAND_ANSWER "OK"

#define CONFIGURATE_COMMAND_ANSWER "OK"

typedef enum {
  NO_EVENT = 0,
  START_CONFIG,
  CHECK_CONFIG,
  ANSWER_CHECK_CONFIG,
  CONFIG_END,
  CONFIGURARE,
  ANSWER_CONFIGURARE,
  ERROR_CONFIG,
  CONNECTED,
  DISCONNECTED,
  CHECK_CONNECTION
} BluetoothEvents;

void SetBluetoothEventTimer(BluetoothEvents event, uint16_t timeOut);
void StopBluetoothEventTimer();
void cleareCounterBluetoothEventTimer();

void setBluetoothEvent(BluetoothEvents event);

void processBluetoothEventTimer();

void processBluetooth();

#endif // _BLUETOOTH_CONFIG