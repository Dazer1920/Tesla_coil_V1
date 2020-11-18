#include "Connection.h"

BluetoothConfigEvent bluetoothAppEvents = NO_EVENT;

uint32_t countConnectionEventTimer = 0, valueCountConnectionEventTimer = 0;
BluetoothConfigEvent ConnectionTimerEvent = NO_EVENT;

void stopConnectEventTimer() {
  countConnectionEventTimer = 0;
  valueCountConnectionEventTimer = 0;
  ConnectionTimerEvent = NO_EVENT;
}

void cleareConterConnectTimer() {
  countConnectionEventTimer = valueCountConnectionEventTimer;
}

void setConnectEventTimer(uint32_t ms, BluetoothConfigEvent event) {
  valueCountConnectionEventTimer = ms;
  ConnectionTimerEvent = event;
  cleareConterConnectTimer();
}

void processConnectionEventTimer() {
  if(ConnectionTimerEvent != NO_EVENT) {
    if(--countConnectionEventTimer == 0) {
      bluetoothAppEvents = ConnectionTimerEvent;
      ConnectionTimerEvent = NO_EVENT;
    }
  }
}


uint8_t countChangeConnection;
Enable isConnected = OFF;

void ChangeConnection() {
  if(countChangeConnection >= 2) {
    stopConnectEventTimer();
    bluetoothAppEvents = DISCONNECTED;
  }
  
  countChangeConnection++;
  setConnectEventTimer(1500, CHANGE_CONNECTION);
}

void Connected() {
  setConnectEventTimer(1500, CHANGE_CONNECTION);
  WriteTemperatureAndPowerValues();
  isConnected = ON;
  countChangeConnection = 0;
}

void Disconnected() {
  stopConnectEventTimer();
  isConnected = OFF;
  
  OffInterrupter();
  
}

void processConnection() {
  switch(bluetoothAppEvents) {
  default: break;
  
  case CHANGE_CONNECTION: bluetoothAppEvents = NO_EVENT; ChangeConnection(); break;
  case DISCONNECTED: bluetoothAppEvents = NO_EVENT; Disconnected(); break;
  case CONNECTED: bluetoothAppEvents = NO_EVENT; Connected(); break;
  }
}