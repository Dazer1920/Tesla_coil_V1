#ifndef _PROCESS_SYSTEM_DATA_H
#define _PROCESS_SYSTEM_DATA_H

#include "System.h"
#include "Serial.h"
#include "Protection.h"
#include "Bluetooth.h"
#include "Connection.h"

#define RX_SYSTEM_DATA_KEY 21

#define RX_IDENTIFICATION_KEY1 54
#define RX_IDENTIFICATION_KEY2 26
#define RX_CHANGE_CONNECTION 23
#define RX_DISCONNECT 25


#define TX_SYSTEM_DATA_KEY 21

#define TX_IDENTIFICATION_KEY1 54
#define TX_IDENTIFICATION_KEY2 26
#define TX_SET_POWER_AND_TEMP 43

#define TX_IDENTIFICATION_KEY1_VAL 0x45
#define TX_IDENTIFICATION_KEY2_VAL 0x76

#define RX_IDENTIFICATION_KEY1_VAL 0x32
#define RX_IDENTIFICATION_KEY2_VAL 0x64
    
void readSystemData(uint8_t *data);

void WriteTemperatureAndPowerValues();

#endif // _PROCESS_SYSTEM_DATA_H