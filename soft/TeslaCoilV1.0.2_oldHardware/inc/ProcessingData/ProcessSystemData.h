#ifndef _PROCESS_SYSTEM_DATA_H
#define _PROCESS_SYSTEM_DATA_H

#include "System.h"
#include "Serial.h"
#include "Protection.h"

#define RX_SYSTEM_DATA_KEY 0x00
#define RX_SYSTEM_DATA_MSK (0x80 | (RX_SYSTEM_DATA_KEY << 5))

#define RX_IDENTIFICATION_KEY1 RX_SYSTEM_DATA_MSK | 0x01
#define RX_IDENTIFICATION_KEY2 RX_SYSTEM_DATA_MSK | 0x02
#define RX_CHANGE_CONNECTION RX_SYSTEM_DATA_MSK | 0x04
#define RX_DISCONNECT RX_SYSTEM_DATA_MSK | 0x05


#define TX_SYSTEM_DATA_KEY 0x00
#define TX_SYSTEM_DATA_MSK 0x80 << (TX_SYSTEM_DATA_KEY << 5)

#define TX_IDENTIFICATION_KEY1 TX_SYSTEM_DATA_MSK | 0x01
#define TX_IDENTIFICATION_KEY2 TX_SYSTEM_DATA_MSK | 0x02
#define TX_SET_TEMP TX_SYSTEM_DATA_MSK | 0x03
#define TX_SET_POWER TX_SYSTEM_DATA_MSK | 0x06
#define TX_SET_TEMP_CPU TX_SYSTEM_DATA_MSK | 0x07

#define TX_IDENTIFICATION_KEY1_VAL 0x45
#define TX_IDENTIFICATION_KEY2_VAL 0x76

#define RX_IDENTIFICATION_KEY1_VAL 0x32
#define RX_IDENTIFICATION_KEY2_VAL 0x64

void setKey1(uint8_t key);
void setKey2(uint8_t key);

#endif // _PROCESS_SYSTEM_DATA_H