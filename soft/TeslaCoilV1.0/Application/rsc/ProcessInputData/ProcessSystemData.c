#include "ProcessSystemData.h"

void processIdentificationKey1(uint8_t key) {
  if(key == RX_IDENTIFICATION_KEY1_VAL) {
    uint8_t frame[] = {TX_SYSTEM_DATA_KEY, TX_IDENTIFICATION_KEY1, TX_IDENTIFICATION_KEY1_VAL};
    SerialWriteFrame(frame, 3);
  }
}

void processIdentificationKey2(uint8_t key) {
  if(key == RX_IDENTIFICATION_KEY2_VAL) {
    uint8_t frame[] = {TX_SYSTEM_DATA_KEY, TX_IDENTIFICATION_KEY2, TX_IDENTIFICATION_KEY2_VAL};
    SerialWriteFrame(frame, 3);
  }
}

extern BluetoothConfigEvent bluetoothAppEvents;

void readSystemData(uint8_t *data) {
  Enable fault = OFF;
  
  switch(data[1]) {
  default: fault = ON; break;
  
  case RX_IDENTIFICATION_KEY1: processIdentificationKey1(data[2]); break;
  case RX_IDENTIFICATION_KEY2: processIdentificationKey2(data[2]); break;
  case RX_CHANGE_CONNECTION: bluetoothAppEvents = CONNECTED; break;
  case RX_DISCONNECT: bluetoothAppEvents = DISCONNECTED; break;
  }
  
  if(fault == OFF) cleareConterConnectTimer();
}

extern Enable isTermoProtect, isLowPower;

void WriteTemperatureAndPowerValues() {
  uint8_t frame[] = {TX_SYSTEM_DATA_KEY, TX_SET_POWER_AND_TEMP, 
  getTempValue() >> 7, getTempValue() & 0x7f, (isTermoProtect == ON)? 1 : 0, 
  getPowerValue() >> 7, getPowerValue() & 0x7f, (isLowPower == ON)? 1 : 0,
  VALUE_RESISTANCE_R1 >> 7, VALUE_RESISTANCE_R1 & 0x7f,
  VALUE_RESISTANCE_R2 >> 7, VALUE_RESISTANCE_R2 & 0x7f};
  
  SerialWriteFrame(frame, 12);
}